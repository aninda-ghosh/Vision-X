///For Image Declaration and Initialisation
IplImage *img1;
IplImage *img2;
IplImage *image;
CvCapture* capture1;
CvCapture* capture2;
int iter;   ///Set the number of iteration
int tolerance = 20; ///Set the tolerance level for threshholding in kmeans algo
int cn = 4; ///Set the number of clusters to classify
int height, width, step, channels;	///Defines the height width and channel of the frame

///Two image defined for operation
Mat OrigIm;
Mat GndIm;
Mat HumanIm;

bool ground_presence = false;
bool stair_presence = false;
bool door_presence = false;
bool human_presence = false;



///variables needed for K-Means Clustering Algorithm
uchar *data;
int *clusx;
int *clusy;
int *var;
int i, j, k, l, h, b;
int *pixel;
int toggle,starting,ending,difference,cnt;
int *ClusterCenter;
int *PrevClusterCenter;
int *ThresholdPoints;
int global_var;
int total_var;
int threshold_var = 20;
int range;
long int sigmanum;
long int sigmaden;

///variable to generate random number generator
time_t  now;    ///checking the systems time for random number generator

/** Global variables */
String face_cascade_name = "/home/aninda/opencv-2.4.9/data/haarcascades/haarcascade_frontalface_default.xml";
String eyes_cascade_name = "/home/xeon/opencv-2.4.9/data/haarcascades/haarcascade_eye.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

///Variables needed for toggle switch for door Stair and Human
int door_toggledetect,door_prevtoggledetect;
int stair_toggledetect,stair_prevtoggledetect;
int human_toggledetect,human_prevtoggledetect;

/*******************************Sound Generation*********************************/
///For Sound Declaration and Initialisation
espeak_POSITION_TYPE position_type;
espeak_AUDIO_OUTPUT output;
char *path=NULL;
int Buflength = 500, Options=0;
void* user_data;
t_espeak_callback *SynthCallback;
espeak_PARAMETER Parm;

char Voice[] = {"default"};
unsigned int position=0, end_position=0, flags=espeakCHARS_AUTO, *unique_identifier;
int TextSize;
int decision;
char *text;
