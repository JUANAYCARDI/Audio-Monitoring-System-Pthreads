/***( Manifest constants for user-defined queuing system  )********************/

#define     BUFSIZE       8     /* number of slots in queues */
#define     NUM_QUEUES    9     /* number of queues */
#define     INTERFACE_Q   0     /* queue 0: interface */
#define     COMM_Q		  1     /* queue 1: communication */
#define     CONTROL_Q     2     /* queue 2: control */
#define     AUDIO_Q1      3     /* queue 3: audio 1 */
#define     AUDIO_Q2      4     /* queue 4: audio 2 */    
#define     AUDIO_Q3      5     /* queue 5: audio 3 */
#define     ENV_Q1        6     /* queue 6: environment 1 */
#define     ENV_Q2        7     /* queue 7: environment 2 */
#define     ENV_Q3        8     /* queue 8: environment 3 */


/***( User-defined message structure )*****************************************/

typedef struct
{
    int   signal;

    //Interface
    int maxNoise;
    int intervalTime;

    //Comm
    int decibels;
    int contact;
    int date1;
    int date2;
    unsigned long long int sensorID;

    //Audio
    int newSound;
    int newInterval;
} msg_t;

/***( Signals )****************************************************/

typedef enum{
  sTurnOn,
  sTurnOff,
  sConf,
  sConfData,
  sNewConf
} TO_INTER;

typedef enum{
  sStartComm,
  sStopComm,
  sAlert,
  sData
}TO_COMM;

typedef enum{
  sStartCtrl,
  sStopCtrl,
  sConfCtrl,
  sNewConfData,
  sInterval
}TO_CONTROL;

typedef enum{
  sStartAudio,
  sStopAudio,
  sSound,
  sNewTime
}TO_AUDIO;

typedef enum{
  sStartEnv,
  sStopEnv
}TO_ENV;

/***( User-defined EFSM states )************************************************/

typedef enum{
    InterIdleOn,
    InterIdleOff,
    Display,
    SendNewConf
} INTER_STATES;

typedef enum{
  ControlOn,
  ControlOff,
  WaitingNewConf
} CTRL_STATES;

typedef enum{
    CommOn,
    CommOff
} COMM_STATES;

typedef enum{
  AudioOff,
  AudioOn
} AUDIO_STATES;