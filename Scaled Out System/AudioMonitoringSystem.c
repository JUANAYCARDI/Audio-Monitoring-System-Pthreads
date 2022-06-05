/*******************************************************************************
*
*  AudioMonitoringSystem.c -      A program implementing the Audio 
*                                 Monitoring System v1 with its most 
*                                 important features
*
*   
*
*******************************************************************************/

#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <unistd.h>
#include    <time.h>
#include    <unistd.h>
#include    <errno.h>
#include    "phtrdsMsgLyr.h"              /* pthreads message layer function
                                             prototypes, constants, structs */


/***( Function prototypes )***********************************************/

static void *pStartStopSystem ( void *arg );     /* Start and Stop System Code */
static void *pAudioSystem ( void *arg );         /* Audio code */
static void *pCommSystem ( void *arg );          /* Communication code */
static void *pControlSystem ( void *arg );       /* Control code */
static void *pInterface ( void *arg );           /* Interface code */
static void *pEnvironment ( void *arg );         /* Environment code */

/***( Thread IDS)*********************************************************/

unsigned long long int audio1TID;
unsigned long long int audio2TID;
unsigned long long int audio3TID;
unsigned long long int env1TID;
unsigned long long int env2TID;
unsigned long long int env3TID;

/***( SDL system creation )***********************************************/
int main ( void ){

  pthread_t   start_tid;               /* Start/Stop System tid*/
  pthread_t   audio1_tid;              /* Audio System 1 tid */
  pthread_t   audio2_tid;              /* Audio System 2 tid */
  pthread_t   audio3_tid;              /* Audio System 3 tid */
  pthread_t   comm_tid;                /* Communication System tid */
  pthread_t   control_tid;             /* Control System tid */
  pthread_t   interface_tid;           /* Interface tid */
  pthread_t   environment1_tid;        /* Environmnet 1 tid */
  pthread_t   environment2_tid;        /* Environmnet 2 tid */
  pthread_t   environment3_tid;        /* Environmnet 3 tid */

  audio1TID = -1;
  audio2TID = -1;
  audio3TID = -1;

  env1TID = -1;
  env2TID = -1;
  env3TID = -1;

  

  /* Create queues */
  initialiseQueues ();

  /* Create threads */
  pthread_create ( &start_tid, NULL, pStartStopSystem, NULL ); 	
  pthread_create ( &interface_tid, NULL, pInterface, NULL );
  pthread_create ( &comm_tid, NULL, pCommSystem, NULL );
  pthread_create ( &audio1_tid, NULL, pAudioSystem, NULL );
  pthread_create ( &audio2_tid, NULL, pAudioSystem, NULL );
  pthread_create ( &audio3_tid, NULL, pAudioSystem, NULL );
  pthread_create ( &control_tid, NULL, pControlSystem, NULL );
  pthread_create ( &environment1_tid, NULL, pEnvironment, NULL );
  pthread_create ( &environment2_tid, NULL, pEnvironment, NULL );
  pthread_create ( &environment3_tid, NULL, pEnvironment, NULL );

  

  /* Wait for threads to finish */
  pthread_join ( start_tid, NULL );
  pthread_join ( interface_tid, NULL );
  pthread_join ( comm_tid, NULL );
  pthread_join ( audio1_tid, NULL );
  pthread_join ( audio2_tid, NULL );
  pthread_join ( audio3_tid, NULL );
  pthread_join ( control_tid, NULL );
  pthread_join ( environment1_tid, NULL );
  pthread_join ( environment2_tid, NULL );
  pthread_join ( environment3_tid, NULL );

  /* Destroy queues */
  destroyQueues ();

  return ( 0 );
}

static void *pStartStopSystem ( void *arg ){
	
  char  line [100]; 
	
  msg_t             OutMsg;
	
  int opt;
  printf ( "----------------------------------------------------------------------------------\n\n" );
  printf ( "To turn on the system press (1)...\n\n" );      
  printf ( "----------------------------------------------------------------------------------\n\n" );
  fflush ( stdout );
  fflush ( stdin );
  fgets(line, sizeof(line), stdin);
  sscanf ( line, "%d", &opt );
	
  OutMsg.signal = (int) InterIdleOn;
  sendMessage ( &(queue [INTERFACE_Q]), OutMsg );
  printf ( "\nInterIdleOn -> INTERFACE\n" );	

  while(opt != 0){
	  printf ( "----------------------------------------------------------------------------------\n\n" );
	  printf ( " To turn off the system press (0), to set a new system configuration press (2)...\n\n" );
	  printf ( "----------------------------------------------------------------------------------\n\n");
      printf ( "%s %25s %30s %30s %40s\n\n", "pInterface","pControl","pAudio","pCommSystem","pEnvironment" );
	  fflush ( stdout );
	  fflush ( stdin );
	  fgets(line, sizeof(line), stdin);
	  sscanf ( line, "%d", &opt );
	  if(opt == 2){
		  OutMsg.signal = (int) sConf;
		  sendMessage ( &(queue [INTERFACE_Q]), OutMsg );
		  OutMsg.signal = (int) sNewConf;
		  sendMessage ( &(queue [INTERFACE_Q]), OutMsg );
		  sleep(15);
	  }
	  else if(opt == 0){
		  OutMsg.signal = (int) sTurnOff;
		  sendMessage ( &(queue [INTERFACE_Q]), OutMsg );
	  }
  }
  return ( NULL );
}

static void *pInterface ( void *arg ){
  
  char  line [100]; 

  INTER_STATES      state,
                    state_next;
  msg_t             InMsg,
	                OutMsg;

  state_next = InterIdleOff;
  
  for ( ; ; ){
	sleep(1);
    state = state_next;
    InMsg = receiveMessage( &(queue [INTERFACE_Q]) );
	
    switch ( state )
    {
      case InterIdleOff:
		
        OutMsg.signal = (int) sStartCtrl;
		sendMessage ( &(queue [CONTROL_Q]), OutMsg );
		printf ( "sStartCtrl -> CONTROL\n" );  
	
        OutMsg.signal = (int) sStartAudio;
		sendMessage ( &(queue [AUDIO_Q1]), OutMsg );
		printf ( "sStartAudio -> AUDIO1\n" );

		OutMsg.signal = (int) sStartAudio;
		sendMessage ( &(queue [AUDIO_Q2]), OutMsg );
		printf ( "sStartAudio -> AUDIO2\n" );

		OutMsg.signal = (int) sStartAudio;
		sendMessage ( &(queue [AUDIO_Q3]), OutMsg );
		printf ( "sStartAudio -> AUDIO3\n" );
	
        OutMsg.signal = (int) sStartComm;
        sendMessage ( &(queue [COMM_Q]), OutMsg );
		printf ( "sStartComm -> COMM\n" );

		OutMsg.signal = (int) sStartEnv;
        sendMessage ( &(queue [ENV_Q1]), OutMsg );
		printf ( "sStartEnv -> ENV1\n" );

		OutMsg.signal = (int) sStartEnv;
        sendMessage ( &(queue [ENV_Q2]), OutMsg );
		printf ( "sStartEnv -> ENV2\n" );

		OutMsg.signal = (int) sStartEnv;
        sendMessage ( &(queue [ENV_Q3]), OutMsg );
		printf ( "sStartEnv -> ENV3\n\n" );
		  
        state_next = InterIdleOn;
        break;
      case InterIdleOn:
        switch ( InMsg.signal )
        {
          case sConf:
            OutMsg.signal = (int) sConfCtrl;
            sendMessage ( &(queue [CONTROL_Q]), OutMsg);
		    printf ( "sConfCtrl -> CONTROL\n" );
            state_next = Display;
            break;
          case sTurnOff:
            OutMsg.signal = (int) sStopCtrl;
			sendMessage ( &(queue [CONTROL_Q]), OutMsg );
		  	printf ( "sStopCtrl -> CONTROL\n" );
			  
	        OutMsg.signal = (int) sStopAudio;
			sendMessage ( &(queue [AUDIO_Q1]), OutMsg );
			printf ( "sStopAudio -> AUDIO1\n" );

			OutMsg.signal = (int) sStopAudio;
			sendMessage ( &(queue [AUDIO_Q2]), OutMsg );
			printf ( "sStopAudio -> AUDIO2\n" );

			OutMsg.signal = (int) sStopAudio;
			sendMessage ( &(queue [AUDIO_Q3]), OutMsg );
			printf ( "sStopAudio -> AUDIO3\n" );
			  
	        OutMsg.signal = (int) sStopComm;
	        sendMessage ( &(queue [COMM_Q]), OutMsg );
			printf ( "sStopComm -> COMM\n\n" );

            state_next = InterIdleOff;
            break;
          default:
            break;
        }
        break;
      case Display:
        switch ( InMsg.signal )
        {
          case sConfData:
			printf ( "Max Noise: %d \n", InMsg.maxNoise);         
			printf ( "Contact: %d\n", InMsg.contact );
			printf ( "Interval Time: %d\n", InMsg.intervalTime );
        	fflush ( stdout );
            state_next = SendNewConf;
            break;
          default:
            break;
        }
        break;
      case SendNewConf:
        switch ( InMsg.signal )
        {
          case sNewConf:
           	printf ( "Enter new max noise: \n" );         
	        fflush ( stdout );
	        fflush ( stdin );
	        sscanf ( line, "%d", &OutMsg.maxNoise );

			printf ( "Enter new contact: \n" );         
	        fflush ( stdout );
	        fflush ( stdin );
	        sscanf ( line, "%d", &OutMsg.contact );

			printf ( "Enter new interval time: \n" );         
	        fflush ( stdout );
	        fflush ( stdin );
	        sscanf ( line, "%d", &OutMsg.intervalTime );

			OutMsg.signal = (int) sNewConfData;

            sendMessage ( &(queue [CONTROL_Q]), OutMsg );
			printf ( "sNewConfData -> CONTROL\n\n" );
            state_next = InterIdleOn;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  return ( NULL );
}

static void *pControlSystem ( void *arg ){
  
  CTRL_STATES state,
              state_next;

  int maxNoise;
  int contact;
  int intervalTime;

  msg_t       InMsg,
			  OutMsg;

  state_next = ControlOff;

  for ( ; ; ){
	sleep(1);
    state = state_next;
    InMsg = receiveMessage( &(queue [CONTROL_Q]) );
    switch ( state )
    {
      case ControlOff:
        switch ( InMsg.signal )
        {
          case sStartCtrl:
            maxNoise = 65;
            contact = 6303421;
            intervalTime = 5;
            state_next = ControlOn;
            break;
          default:
            break;
        }
        break;
      case ControlOn:
        switch ( InMsg.signal )
        {
          case sConfCtrl:

            OutMsg.signal = (int) sConfData;
            OutMsg.maxNoise = maxNoise;
            OutMsg.contact = contact;
            OutMsg.intervalTime = intervalTime;

            sendMessage ( &(queue [INTERFACE_Q]), OutMsg );
			printf ( "%30s\n\n", "sConfData -> INTERFACE" );
            state_next = WaitingNewConf;
            break;
          case sInterval:
            if (InMsg.decibels > maxNoise){
              
              OutMsg.signal = (int) sAlert;
              OutMsg.decibels = InMsg.decibels;
              OutMsg.contact = InMsg.contact;
              OutMsg.date1 = InMsg.date1;
              OutMsg.date2 = InMsg.date2;
			  OutMsg.sensorID = InMsg.sensorID;

              sendMessage ( &(queue [COMM_Q]), OutMsg );
			  printf ("%41s\n\n", "sAlert-> COMM" );
			}
            state_next = ControlOn;
            break;
          case sStopCtrl:
            state_next = ControlOff;
            break;
          default:
            break;
        }
        break;
      case WaitingNewConf:
        switch ( InMsg.signal )
        {
          case sNewConfData:

            maxNoise = InMsg.maxNoise;
            contact = InMsg.contact;
            intervalTime = InMsg.intervalTime;

            OutMsg.signal = (int) sNewTime;
            OutMsg.intervalTime = intervalTime;

            sendMessage ( &(queue [AUDIO_Q1]), OutMsg );
			printf ( "%40s\n\n", "sNewTime -> AUDIO1" );

			sendMessage ( &(queue [AUDIO_Q2]), OutMsg );
			printf ( "%40s\n\n", "sNewTime -> AUDIO2" );

			sendMessage ( &(queue [AUDIO_Q3]), OutMsg );
			printf ( "%40s\n\n", "sNewTime -> AUDIO3" );
            state_next = ControlOn;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  return ( NULL );
}

static void *pAudioSystem ( void *arg ){
  
  AUDIO_STATES  state,
                state_next;                            

  int decibels;
  int intervalTime;
  int date1;
  int date2;

  msg_t       InMsg,
			  OutMsg;

  unsigned long long int self;
  self = pthread_self();

  if (audio1TID == -1){
	  audio1TID = self;
  }
  else if (audio2TID == -1){
	  audio2TID = self;
  }
  else if (audio3TID == -1){
	  audio3TID = self;
  }
  state_next = AudioOff;

  for ( ; ; ){
	sleep(1);
    state = state_next;
    if (self == audio1TID){
	  	InMsg = receiveMessage( &(queue [AUDIO_Q1]) );  
	}
	else if(self == audio2TID){
		InMsg = receiveMessage( &(queue [AUDIO_Q2]) );    
	}
	else if(self == audio3TID){
		InMsg = receiveMessage( &(queue [AUDIO_Q3]) );  
	}
    switch ( state )
    {
      case AudioOff:
        switch ( InMsg.signal )
        {
          case sStartAudio:
            intervalTime = 3;
            state_next = AudioOn;
            break;
          default:
            break;
        }    
        break;
      case AudioOn:
        switch ( InMsg.signal )
        {
          case sSound:
            decibels = InMsg.newSound;
            date1 = (int)time(NULL);
            date2 = date1 + intervalTime;

            OutMsg.signal = (int) sInterval;
            OutMsg.decibels = decibels;
            OutMsg.date1 = date1;
            OutMsg.date2 = date2;
			OutMsg.sensorID = self;

            sendMessage ( &(queue [CONTROL_Q]), OutMsg );
			printf ( "%81s\n\n", "sInterval -> CONTROL" );
			fflush ( stdout );

			OutMsg.signal = (int) sStartEnv;
			if (self == audio1TID){
				sendMessage ( &(queue [ENV_Q1]), OutMsg );	
			}  
			else if (self == audio2TID){
				sendMessage ( &(queue [ENV_Q2]), OutMsg );	
			} 
			else{
				sendMessage ( &(queue [ENV_Q3]), OutMsg );
			}
        	
			  
            state_next = AudioOn;
            break;
          case sNewTime:
            intervalTime = InMsg.intervalTime;
            state_next = AudioOn;
            break;
          case sStopAudio:
            state_next = AudioOff;
            break;
          default:
            break;
        }    
        break;
      default:
        break;
    }
  }
  return ( NULL );
}

static void *pCommSystem ( void *arg ){
    
  COMM_STATES state,
              state_next;

  msg_t       InMsg;

  state_next = CommOff;

  for ( ; ; ){
	sleep(1);
    state = state_next;
    InMsg = receiveMessage( &(queue [COMM_Q]) );
    switch ( state )
    {
      case CommOff:
        switch ( InMsg.signal )
        {
          case sStartComm:

            state_next = CommOn;
            break;
          default:
            break;
        }
      case CommOn:
        switch ( InMsg.signal )
        {
          case sAlert:
			printf ( "%118s\n", "Violation to the decibel limit.");
            printf ( "%97s%d \n", "Decibels: ",InMsg.decibels);
			printf ( "%95s%d\n", "Date 1: ",InMsg.date1 );
			printf ( "%95s%d\n", "Date 2: ",InMsg.date2 );
			printf ( "%98s%llu\n\n", "Sensor ID: ",InMsg.sensorID);
        	fflush ( stdout );

            state_next = CommOn;
            break;
          case sStopComm:
            state_next = CommOff;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  return ( NULL );
}

static void *pEnvironment ( void *arg ){

  msg_t      InMsg,
			 OutMsg;
  unsigned long long int  self;
  self = pthread_self();
  if (env1TID == -1){
	  env1TID = self;
  }
  else if (env2TID == -1){
	  env2TID = self;
  }
  else if (env3TID == -1){
	  env3TID = self;
  }
  for ( ; ; ){
	  sleep(3);
	  if (self == env1TID){
	  	InMsg = receiveMessage( &(queue [ENV_Q1]) );  
	  }
	  else if(self == env2TID){
		InMsg = receiveMessage( &(queue [ENV_Q2]) );    
	  }
	  else{
		InMsg = receiveMessage( &(queue [ENV_Q3]) );  
	  }
	  
	  if (InMsg.signal == sStartEnv){
			
			OutMsg.signal = (int) sSound;
			OutMsg.newSound = (int) (rand() % (90 + 1 - 1) + 1);
		    if (self == env1TID){
			  sendMessage ( &(queue [AUDIO_Q1]), OutMsg );
			  printf ( "%143s\n" , "sSound -> AUDIO1" );
			  printf ( "%137s%d\n", "Decibels: ",OutMsg.newSound );
			}
			else if(self == env2TID){
			  sendMessage ( &(queue [AUDIO_Q2]), OutMsg );
			  printf ( "%143s\n" , "sSound -> AUDIO2" );
			  printf ( "%137s%d\n", "Decibels: ",OutMsg.newSound );
			}
		  	else if(self == env3TID){
			  sendMessage ( &(queue [AUDIO_Q3]), OutMsg );
			  printf ( "%143s\n" , "sSound -> AUDIO3" );
			  printf ( "%137s%d\n", "Decibels: ",OutMsg.newSound );
			}
			
		    
	  }
  }
  return ( NULL );
}