/*###############################################################################
#
# Copyright 2021 NVIDIA Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
###############################################################################*/

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "nvAR.h"
#include "nvAR_defs.h"
#include "nvCVOpenCV.h"
#include "opencv2/opencv.hpp"
#include "MeshRenderer.h"
#include "PipeServer.cpp"

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif // WIN32_LEAN_AND_MEAN
  #include <Windows.h>
  #define strcasecmp _stricmp
#else
  #include <sys/stat.h>
#endif // _WIN32

#if CV_MAJOR_VERSION >= 4
  #define CV_CAP_PROP_FPS           cv::CAP_PROP_FPS
  #define CV_CAP_PROP_FRAME_COUNT   cv::CAP_PROP_FRAME_COUNT
  #define CV_CAP_PROP_FRAME_HEIGHT  cv::CAP_PROP_FRAME_HEIGHT
  #define CV_CAP_PROP_FRAME_WIDTH   cv::CAP_PROP_FRAME_WIDTH
  #define CV_CAP_PROP_POS_FRAMES    cv::CAP_PROP_POS_FRAMES
  #define CV_INTER_AREA             cv::INTER_AREA
  #define CV_INTER_LINEAR           cv::INTER_LINEAR
#endif // CV_MAJOR_VERSION

#ifndef M_PI
  #define M_PI                      3.1415926535897932385
#endif /* M_PI */
#ifndef M_2PI
  #define M_2PI                     6.2831853071795864769
#endif /* M_2PI */
#ifndef M_PI_2
  #define M_PI_2                    1.5707963267948966192
#endif /* M_PI_2 */
#define D_RADIANS_PER_DEGREE        (M_PI / 180.)
#define F_PI                        ((float)M_PI)
#define F_PI_2                      ((float)M_PI_2)
#define F_2PI                       ((float)M_2PI)
#define F_RADIANS_PER_DEGREE        (float)(M_PI / 180.)
#define CTL(x)                      ((x) & 0x1F)
#define HELP_REQUESTED              411

#define BAIL_IF_ERR(err)            do { if ((int)(err) != 0)     { goto bail;             } } while(0)
#define BAIL_IF_NULL(x, err, code)  do { if ((void*)(x) == NULL)  { err = code; goto bail; } } while(0)
#define BAIL_IF_CUERR(cu, err)      do { if (cudaSuccess != (cu)) { err = NvFromCuErr(cu); } } while(0)
#define BAIL(err, code)             do {                            err = code; goto bail;   } while(0)

#define DEFAULT_CODEC         "avc1"

#define NUM_CAMERA_INTRINSIC_PARAMS 3

/********************************************************************************
 * Command-line arguments
 ********************************************************************************/

bool
    FLAG_loop               = false,
    FLAG_verbose            = true;
std::string
    FLAG_camRes;
int
    //FLAG_filter can be remained
    FLAG_filter             = NVAR_TEMPORAL_FILTER_FACE_BOX
                            | NVAR_TEMPORAL_FILTER_FACIAL_LANDMARKS
                            | NVAR_TEMPORAL_FILTER_FACE_ROTATIONAL_POSE
                            | NVAR_TEMPORAL_FILTER_FACIAL_EXPRESSIONS
                            | NVAR_TEMPORAL_FILTER_FACIAL_GAZE,
                            //| NVAR_TEMPORAL_FILTER_ENHANCE_EXPRESSIONS,
    FLAG_camID              = 0;            


/********************************************************************************
 * ParseMyArgs
 ********************************************************************************/
static bool GetFlagArgVal(const char *flag, const char *arg, const char **val) {
  if (*arg != '-')
    return false;
  while (*++arg == '-')
    continue;
  const char *s = strchr(arg, '=');
  if (s == nullptr) {
    if (strcmp(flag, arg))
      return false;
    *val = nullptr;
    return true;
  }
  unsigned n = (unsigned)(s - arg);
  if ((strlen(flag) != n) || (strncmp(flag, arg, n) != 0))
    return false;
  *val = s + 1;
  return true;
}

static bool GetFlagArgVal(const char *flag, const char *arg, std::string *val) {
  const char *valStr;
  if (!GetFlagArgVal(flag, arg, &valStr)) return false;
  val->assign(valStr ? valStr : "");
  return true;
}

bool GetFlagArgVal(const char *flag, const char *arg, long *val) {
  const char *valStr;
  bool success = GetFlagArgVal(flag, arg, &valStr);
  if (success)
    *val = strtol(valStr, nullptr, 0);
  return success;
}

static bool GetFlagArgVal(const char *flag, const char *arg, int *val) {
  long longVal;
  bool success = GetFlagArgVal(flag, arg, &longVal);
  if (success)
    *val = (int)longVal;
  return success;
}

static int ParseMyArgs(int argc, char **argv) {
  
  int errs = 0;
  for (--argc, ++argv; argc--; ++argv) {
    bool help;
    const char *arg = *argv;

    if (arg[0] != '-') {
      continue;
    } 

    else if ((arg[1] == '-') && (
      GetFlagArgVal("cam_id", arg, &FLAG_camID) ||
      GetFlagArgVal("cam_res", arg, &FLAG_camRes)        
    )) {
      continue;
    } 
    
  }
  return errs;
}


enum {
  myErrNone = 0,
  myErrShader = -1,
  myErrProgram = -2,
  myErrTexture = -3,
};


class MyTimer {
public:
  MyTimer()     { dt = dt.zero();                                      }  /**< Clear the duration to 0. */
  void start()  { t0 = std::chrono::high_resolution_clock::now();      }  /**< Start  the timer. */
  void pause()  { dt = std::chrono::high_resolution_clock::now() - t0; }  /**< Pause  the timer. */
  void resume() { t0 = std::chrono::high_resolution_clock::now() - dt; }  /**< Resume the timer. */
  void stop()   { pause();                                             }  /**< Stop   the timer. */
  double elapsedTimeFloat() const {
    return std::chrono::duration<double>(dt).count();
  } /**< Report the elapsed time as a float. */
private:
  std::chrono::high_resolution_clock::time_point t0;
  std::chrono::high_resolution_clock::duration dt;
};

inline NvCV_Status NvFromAppErr(int appErr) { return (NvCV_Status)appErr; }


class App {
public:

  App() {}
  ~App() { stop(); }

  NvCV_Status run();
  NvCV_Status stop();
  NvCV_Status setInputCamera(int index, const std::string& resStr); // open immediately
  NvCV_Status set(int codec, double fps, unsigned width, unsigned height);  // deferred open
  NvCV_Status init();
  NvCV_Status initMLPExpressions();
  NvCV_Status normalizeExpressionsWeights();
  NvCV_Status updateCamera();
  void getFPS();

  const char *getErrorStringFromCode(NvCV_Status err);

  struct Pose {
    NvAR_Quaternion rotation;
    NvAR_Vector3f translation;
    float* data() { return &rotation.x; }
    const float* data() const { return &rotation.x; }
  };
  CUstream          _stream = 0;
  cv::Mat           _ocvSrcImg, _ocvDstImg; // _ocvSrcImg is allocated, _ocvDstImg is just a wrapper
  cv::VideoCapture  _vidIn{};
  cv::VideoWriter   _vidOut{};
  double            _frameRate;
  NvAR_FaceMesh     _arMesh { nullptr, 0, nullptr, 0 };
  NvAR_FeatureHandle _featureHan{};
  NvCVImage         _srcImg, _compImg, _srcGpu; // wrapper, alloced, alloced, alloced
  Pose              _pose;
  float             _cameraIntrinsicParams[NUM_CAMERA_INTRINSIC_PARAMS];
  std::string       _inFile, _outFile;
  std::vector<NvAR_Rect> _outputBboxData;
  NvAR_BBoxes        _outputBboxes;
  std::vector<float> _expressions, _expressionZeroPoint, _expressionScale, _expressionExponent, _eigenvalues,
                     _landmarkConfidence;
  std::vector<NvAR_Point2f> _landmarks;
  std::vector<NvAR_Vector3f> _vertices;
  std::vector< NvAR_Vector3u16> _triangles;
  unsigned          _videoWidth, _videoHeight,
                    _compWidth, _compHeight, _eigenCount, _exprCount, _landmarkCount,
                    _exprMode, _filtering;
  unsigned          _poseMode = 0;
  bool              _enableCheekPuff = false;
  static const char _windowTitle[], *_exprAbbr[][4];
  MyTimer           _timer;
  bool              _showFPS;
  bool              _cameraNeedsUpdate;
  double            _frameTime;
  float             _globalExpressionParam;

  PipeServer pipeServer;

  enum {
    VIEW_MESH   = (1 << 0),
    VIEW_IMAGE  = (1 << 1),
    VIEW_PLOT   = (1 << 2),
    VIEW_LM     = (1 << 3)
  };
  enum {
    APP_ERR_GENERAL = 1,
    APP_ERR_OPEN,
    APP_ERR_READ,
    APP_ERR_WRITE,
    APP_ERR_INIT,
    APP_ERR_RUN,
    APP_ERR_EFFECT,
    APP_ERR_PARAM,
    APP_ERR_UNIMPLEMENTED,
    APP_ERR_MISSING,
    APP_ERR_VIDEO,
    APP_ERR_IMAGE_SIZE,
    APP_ERR_NOT_FOUND,
    APP_ERR_FACE_MODEL,
    APP_ERR_GLFW_INIT,
    APP_ERR_GL_INIT,
    APP_ERR_RENDER_INIT,
    APP_ERR_GL_RESOURCE,
    APP_ERR_GL_GENERAL,
    APP_ERR_FACE_FIT,
    APP_ERR_NO_FACE,
    APP_ERR_CANCEL,
    APP_ERR_CAMERA,
    APP_ERR_ARG_PARSE,
    APP_ERR_EOF
  };
};
const char App::_windowTitle[] = "Expression App";
const char *App::_exprAbbr[][4] = {
  { "BROW", "DOWN", "LEFT", NULL    },    // 0  browDown_L
  { "BROW", "DOWN", "RIGHT",NULL    },    // 1  browDown_R
  { "BROW", "INNR", "UP",   "LEFT"  },    // 2  browInnerUp_L
  { "BROW", "INNR", "UP",   "RIGHT" },    // 3  browInnerUp_R
  { "BROW", "OUTR", "UP",   "LEFT"  },    // 4  browOuterUp_L
  { "BROW", "OUTR", "UP",   "RIGHT" },    // 5  browOuterUp_R
  { "CHEE", "PUFF", "LEFT", NULL    },    // 6  cheekPuff_L
  { "CHEE", "PUFF", "RIGHT",NULL    },    // 7  cheekPuff_R
  { "CHEE", "SQNT", "LEFT", NULL    },    // 8  cheekSquint_L
  { "CHEE", "SQNT", "RIGHT",NULL    },    // 9  cheekSquint_R
  { "EYE",  "BLNK", "LEFT", NULL    },    // 10 eyeBlink_L
  { "EYE",  "BLNK", "RIGHT",NULL    },    // 11 eyeBlink_R
  { "EYE",  "LOOK", "DOWN", "LEFT"  },    // 12 eyeLookDown_L
  { "EYE",  "LOOK", "DOWN", "RIGHT" },    // 13 eyeLookDown_R
  { "EYE",  "LOOK", "IN",   "LEFT"  },    // 14 eyeLookIn_L
  { "EYE",  "LOOK", "IN",   "RIGHT" },    // 15 eyeLookIn_R
  { "EYE",  "LOOK", "OUT",  "LEFT"  },    // 16 eyeLookOut_L
  { "EYE",  "LOOK", "OUT",  "RIGHT" },    // 17 eyeLookOut_R
  { "EYE",  "LOOK", "UP",   "LEFT"  },    // 18 eyeLookUp_L
  { "EYE",  "LOOK", "UP",   "RIGHT" },    // 19 eyeLookUp_R
  { "EYE",  "SQNT", "LEFT", NULL    },    // 20 eyeSquint_L
  { "EYE",  "SQNT", "RIGHT",NULL    },    // 21 eyeSquint_R
  { "EYE",  "WIDE", "LEFT", NULL    },    // 22 eyeWide_L
  { "EYE",  "WIDE", "RIGHT",NULL    },    // 23 eyeWide_R
  { "JAW",  "FWD",  NULL,   NULL    },    // 24 jawForward
  { "JAW",  "LEFT", NULL,   NULL    },    // 25 jawLeft
  { "JAW",  "OPEN", NULL,   NULL    },    // 26 jawOpen
  { "JAW",  "RIGHT",NULL   ,NULL    },    // 27 jawRight
  { "MOUT", "CLOS", NULL,   NULL    },    // 28 mouthClose
  { "MOUT", "DMPL", "LEFT", NULL    },    // 29 mouthDimple_L
  { "MOUT", "DMPL", "RIGHT",NULL    },    // 30 mouthDimple_R
  { "MOUT", "FRWN", "LEFT", NULL    },    // 31 mouthFrown_L
  { "MOUT", "FRWN", "RIGHT",NULL    },    // 32 mouthFrown_R
  { "MOUT", "FUNL", NULL,   NULL    },    // 33 mouthFunnel
  { "MOUT", "LEFT", NULL,   NULL    },    // 34 mouthLeft
  { "MOUT", "LOWR", "DOWN", "LEFT"  },    // 35 mouthLowerDown_L
  { "MOUT", "LOWR", "DOWN", "RIGHT" },    // 36 mouthLowerDown_R
  { "MOUT", "PRES", "LEFT", NULL    },    // 37 mouthPress_L
  { "MOUT", "PRES", "RIGHT",NULL    },    // 38 mouthPress_R
  { "MOUT", "PUKR", NULL,   NULL    },    // 39 mouthPucker
  { "MOUT", "RIGHT",NULL,   NULL    },    // 40 mouthRight
  { "MOUT", "ROLL", "LOWR", NULL    },    // 41 mouthRollLower
  { "MOUT", "ROLL", "UPPR", NULL    },    // 41 mouthRollUpper
  { "MOUT", "SHRG", "LOWR", NULL    },    // 43 mouthShrugLower
  { "MOUT", "SHRG", "UPPR", NULL    },    // 44 mouthShrugUpper
  { "MOUT", "SMIL", "LEFT", NULL    },    // 45 mouthSmile_L
  { "MOUT", "SMIL", "RIGHT",NULL    },    // 46 mouthSmile_R
  { "MOUT", "STRH", "LEFT", NULL    },    // 47 mouthStretch_L
  { "MOUT", "STRH", "RIGHT",NULL    },    // 48 mouthStretch_R
  { "MOUT", "UPPR", "UP",   "LEFT"  },    // 49 mouthUpperUp_L
  { "MOUT", "UPPR", "UP",   "RIGHT" },    // 50 mouthUpperUp_R
  { "NOSE", "SNER", "LEFT", NULL    },    // 51 noseSneer_L
  { "NOSE", "SNER", "RIGHT",NULL    },    // 52 noseSneer_R
};



NvCV_Status App::setInputCamera(int index, const std::string& resStr) {
  if (!_vidIn.open(index))
    return NvFromAppErr(APP_ERR_OPEN);
  if (!resStr.empty()) {
    int n, width, height;
    n = sscanf(resStr.c_str(), "%d%*[xX]%d", &width, &height);
    switch (n) {
      case 2:
        break;  // We have read both width and height
      case 1:
        height = width;
        width = (int)(height * (4. / 3.) + .5);
        break;
      default:
        height = 0;
        width = 0;
        break;
    }
    if (width)  _vidIn.set(CV_CAP_PROP_FRAME_WIDTH,  width);
    if (height) _vidIn.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    _inFile = "webcam";
    _inFile += std::to_string(index);
  }
  _videoWidth  = (unsigned)_vidIn.get(CV_CAP_PROP_FRAME_WIDTH);
  _videoHeight = (unsigned)_vidIn.get(CV_CAP_PROP_FRAME_HEIGHT);
  _frameRate = _vidIn.get(CV_CAP_PROP_FPS);
  // Rounding the frame rate is required because OpenCV does not support all frame rates when writing video
  static const int fps_precision = 1000;
  _frameRate = static_cast<int>((_frameRate + 0.5) * fps_precision) / static_cast<double>(fps_precision);
  if (FLAG_verbose)
    printf("Camera capture resolution set to %dx%d @ %4.1f fps\n", _videoWidth, _videoHeight, _frameRate);
  return NVCV_SUCCESS;
}


NvCV_Status App::stop() {

  if (_vidOut.isOpened()) _vidOut.release();
  if (_vidIn.isOpened())  _vidIn.release();
  if (_featureHan)        NvAR_Destroy(_featureHan);
  _featureHan = nullptr;
  _inFile.clear();
  _outFile.clear();
  pipeServer.release();
  return NVCV_SUCCESS;
}


const char *App::getErrorStringFromCode(NvCV_Status err) {
  int intErr = (int)err;
  if (intErr > 0) {
    struct LUTEntry { int code; const char *string; };
    static const struct LUTEntry lut[] {
      { APP_ERR_GENERAL,        "General application error" },
      { APP_ERR_READ,           "Read error" },
      { APP_ERR_WRITE,          "Write error" },
      { APP_ERR_INIT,           "Initialization error" },
      { APP_ERR_RUN,            "Run error" },
      { APP_ERR_EFFECT,         "Error creating an effect" },
      { APP_ERR_PARAM,          "Parameter error" },
      { APP_ERR_UNIMPLEMENTED,  "Unimplemented" },
      { APP_ERR_MISSING,        "Something is missing" },
      { APP_ERR_VIDEO,          "Video error" },
      { APP_ERR_IMAGE_SIZE,     "Image size error" },
      { APP_ERR_NOT_FOUND,      "Not found" },
      { APP_ERR_FACE_MODEL,     "Face model error" },
      { APP_ERR_GLFW_INIT,      "Error initializing GLFW" },
      { APP_ERR_GL_INIT,        "Error initializing OpenGL" },
      { APP_ERR_RENDER_INIT,    "Error initializing the renderer" },
      { APP_ERR_GL_RESOURCE,    "OpenGL resource error" },
      { APP_ERR_GL_GENERAL,     "General OpenGL error" },
      { APP_ERR_FACE_FIT,       "Face fit error" },
      { APP_ERR_NO_FACE,        "No face was found" },
      { APP_ERR_CANCEL,         "The operation has been canceled" },
      { APP_ERR_CAMERA,         "Camera error" },
      { APP_ERR_ARG_PARSE,      "Argument parsing error" },
    };
    for (const LUTEntry *p = lut; p != &lut[sizeof(lut) / sizeof(lut[0])]; ++p)
      if (intErr == p->code)
        return p->string;
  }
  return NvCV_GetErrorStringFromCode(err);
}

char *g_nvARSDKPath = NULL;


NvCV_Status ResizeNvCVImage(const NvCVImage *src, NvCVImage *dst) {
  int interpolation = ((double)dst->width * dst->height / (src->width * src->height) < 1.) ?
                      CV_INTER_AREA : CV_INTER_LINEAR;
  cv::Mat ocvSrc, ocvDst;
  CVWrapperForNvCVImage(src, &ocvSrc);
  CVWrapperForNvCVImage(dst, &ocvDst);
  cv::resize(ocvSrc, ocvDst, ocvDst.size(), 0, 0, interpolation);
  return NVCV_SUCCESS;
}


NvCV_Status App::initMLPExpressions() {
  const unsigned landmarkCount = 126;
  NvCV_Status err;

  // Initialize AR effect
  if (_featureHan) {
    return NVCV_SUCCESS;
  }
  BAIL_IF_ERR(err = NvAR_Create(NvAR_Feature_FaceExpressions, &_featureHan));
  BAIL_IF_ERR(err = NvAR_SetCudaStream(_featureHan, NvAR_Parameter_Config(CUDAStream), _stream));
  BAIL_IF_ERR(err = NvAR_SetU32(_featureHan, NvAR_Parameter_Config(Temporal), _filtering));
  BAIL_IF_ERR(err = NvAR_SetU32(_featureHan, NvAR_Parameter_Config(PoseMode), _poseMode));
  BAIL_IF_ERR(err = NvAR_SetU32(_featureHan, NvAR_Parameter_Config(EnableCheekPuff), _enableCheekPuff));
  BAIL_IF_ERR(err = NvAR_Load(_featureHan));
  _outputBboxData.assign(25, { 0.f, 0.f, 0.f, 0.f });
  _outputBboxes.boxes = _outputBboxData.data();
  _outputBboxes.max_boxes = (uint8_t)_outputBboxData.size();
  _outputBboxes.num_boxes = 0;
  BAIL_IF_ERR(err = NvAR_SetObject(_featureHan, NvAR_Parameter_Output(BoundingBoxes), &_outputBboxes, sizeof(NvAR_BBoxes)));
  _landmarks.resize(landmarkCount);
  BAIL_IF_ERR(err = NvAR_SetObject(_featureHan, NvAR_Parameter_Output(Landmarks), _landmarks.data(), sizeof(NvAR_Point2f)));
  _landmarkConfidence.resize(landmarkCount);
  BAIL_IF_ERR(err = NvAR_SetF32Array(_featureHan, NvAR_Parameter_Output(LandmarksConfidence), _landmarkConfidence.data(), landmarkCount));
  BAIL_IF_ERR(err = NvAR_GetU32(_featureHan, NvAR_Parameter_Config(ExpressionCount), &_exprCount));
  _expressions.resize(_exprCount);
  _expressionZeroPoint.resize(_exprCount, 0.0f);
  _expressionScale.resize(_exprCount, 1.0f);
  _expressionExponent.resize(_exprCount, 1.0f);
  BAIL_IF_ERR(err = NvAR_SetF32Array(_featureHan, NvAR_Parameter_Output(ExpressionCoefficients), _expressions.data(), _exprCount));
  BAIL_IF_ERR(err = NvAR_SetObject(_featureHan, NvAR_Parameter_Input(Image), &_srcGpu, sizeof(NvCVImage)));
  // Heuristic for focal length if it is not known
  _cameraIntrinsicParams[0] = static_cast<float>(_srcGpu.height);         // focal length
  _cameraIntrinsicParams[1] = static_cast<float>(_srcGpu.width) / 2.0f;   // cx
  _cameraIntrinsicParams[2] = static_cast<float>(_srcGpu.height) / 2.0f;  // cy
  BAIL_IF_ERR(err = NvAR_SetF32Array(_featureHan, NvAR_Parameter_Input(CameraIntrinsicParams), _cameraIntrinsicParams,
                                     NUM_CAMERA_INTRINSIC_PARAMS));
  BAIL_IF_ERR(err = NvAR_SetObject(_featureHan, NvAR_Parameter_Output(Pose), &_pose.rotation, sizeof(NvAR_Quaternion)));
  BAIL_IF_ERR(err = NvAR_SetObject(_featureHan, NvAR_Parameter_Output(PoseTranslation), &_pose.translation, sizeof(NvAR_Vector3f)));


bail:
  return err;
}

NvCV_Status App::normalizeExpressionsWeights() {
  assert(_expressions.size() == _exprCount);
  assert(_expressionScale.size() == _exprCount);
  assert(_expressionZeroPoint.size() == _exprCount);
  for (size_t i = 0; i < _exprCount; i++)  {
    float tempExpr = _expressions[i];
    _expressions[i] = 1.0f - (std::pow(
        1.0f - (std::max(_expressions[i] - _expressionZeroPoint[i], 0.0f) * _expressionScale[i]),
        _expressionExponent[i]));
    _expressions[i] = _globalExpressionParam * _expressions[i] + (1.0f - _globalExpressionParam) * tempExpr;
  }
  return NVCV_SUCCESS;
}


NvCV_Status App::init() {
  NvCV_Status err;
  std::string path;

  _compWidth    = _videoWidth;
  _compHeight   = _videoHeight;
  _showFPS      = false;
  _frameTime    = 0.f;
  _exprMode     = 0;
  _poseMode     = 0;
  _enableCheekPuff = false;
  _featureHan   = nullptr;
  _filtering    = FLAG_filter;
  _globalExpressionParam  = 1.0f;
  _cameraIntrinsicParams[0] = 0.0f;
  _cameraIntrinsicParams[1] = 0.0f;
  _cameraIntrinsicParams[2] = 0.0f;
  _cameraNeedsUpdate = true;

  pipeServer.createPipe();
  //printf("connected to a client!");


  BAIL_IF_ERR(err = NvCVImage_Alloc(&_srcGpu,    _videoWidth,   _videoHeight, NVCV_BGR,  NVCV_U8, NVCV_CHUNKY, NVCV_GPU, 1));
  BAIL_IF_ERR(err = NvCVImage_Alloc(&_srcImg,    _videoWidth,   _videoHeight, NVCV_BGR,  NVCV_U8, NVCV_CHUNKY, NVCV_CPU_PINNED, 0));
  BAIL_IF_ERR(err = NvCVImage_Alloc(&_compImg,   _compWidth,   _compHeight,   NVCV_BGR,  NVCV_U8, NVCV_CHUNKY, NVCV_CPU, 0));
  CVWrapperForNvCVImage(&_compImg, &_ocvDstImg);
  CVWrapperForNvCVImage(&_srcImg,  &_ocvSrcImg);

  BAIL_IF_ERR(err = initMLPExpressions());

bail:
  return err;
}


NvCV_Status App::run() {
  NvCV_Status err = NVCV_SUCCESS;
  NvCVImage tmpImg, view;

  for (unsigned frameCount = 0;; ++frameCount) {
    if (!_vidIn.read(_ocvSrcImg) || _ocvSrcImg.empty()) {
      if (!frameCount) return NvFromAppErr(APP_ERR_VIDEO);  // No frames in video
      if (!FLAG_loop)  return NvFromAppErr(APP_ERR_EOF);    // Video has completed
      _vidIn.set(CV_CAP_PROP_POS_FRAMES, 0);                // Rewind, because loop mode has been selected
      --frameCount;                                         // Account for the wasted frame
      continue;                                             // Read the first frame again
    }

    BAIL_IF_ERR(err = NvCVImage_Transfer(&_srcImg, &_srcGpu, 1.f, _stream, nullptr));
    BAIL_IF_ERR(err = NvAR_Run(_featureHan));
    unsigned isFaceDetected = (_outputBboxes.num_boxes > 0) ? 0xFF : 0;
    
    normalizeExpressionsWeights();
    
    vector<float> headrot = {_pose.rotation.x, _pose.rotation.y, _pose.rotation.z, _pose.rotation.w};
    pipeServer.sendMessage(_expressions, headrot);
    
    if (_vidOut.isOpened())
      _vidOut.write(_ocvDstImg);
  }
bail:
  return err;
}

void App::getFPS() {
  const float timeConstant = 16.f;
  _timer.stop();
  float t = (float)_timer.elapsedTimeFloat();
  if (t < 100.f) {
    if (_frameTime)
      _frameTime += (t - _frameTime) * (1.f / timeConstant);  // 1 pole IIR filter
    else
      _frameTime = t;
  }
  else {            // Ludicrous time interval; reset
    _frameTime = 0.f;  // WAKE UP
  }
  _timer.start();
}

/********************************************************************************
 * main
 ********************************************************************************/

int main(int argc, char **argv) {
  NvCV_Status err = NVCV_SUCCESS;
  App app;

  int nErrs;

  if (0 != (nErrs = ParseMyArgs(argc, argv))) {
    printf("ERROR: argument syntax\n");
    BAIL(err, NVCV_ERR_PARSE);
  }

  // Input from a webcam, #0
  err = app.setInputCamera(FLAG_camID, FLAG_camRes);
  if (NVCV_SUCCESS != err) {
    printf("ERROR: cam0: %s\n", app.getErrorStringFromCode(err));
    goto bail;
  }

  
  err = app.init();
  BAIL_IF_ERR(err);

  if (FLAG_verbose) printf("prepare to run\n");
  err = app.run();
  switch ((int)err) {
    case App::APP_ERR_CANCEL: // The user stopped
    case App::APP_ERR_EOF:    // The end of the file was reached.
      err = NVCV_SUCCESS;
      break;
    default:
      break;
  }
  BAIL_IF_ERR(err);

bail:
  if (err)
    printf("ERROR: %s\n", app.getErrorStringFromCode(err));
  return (int)err;
}
