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

static void *pAudioSystem ( void *arg );     /* Audio code */
static void *pCommSystem ( void *arg );      /* Communication code */
static void *pControlSystem ( void *arg );   /* Control code */
static void *pInterface ( void *arg );       /* Interface code */
static void *pEnvironment ( void *arg );     /* Environment code */

/***( SDL system creation )***********************************************/
int main ( void ){

  pthread_t   audio_tid;               /* Audio System tid */
  pthread_t   comm_tid;                /* Communication System tid */
  pthread_t   control_tid;             /* Control System tid */
  pthread_t   interface_tid;           /* Interface tid */
  pthread_t   environment_tid;         /* Environmnet tid */

  /* Create queues */
  initialiseQueues ();

  /* Create threads */
  pthread_create ( &interface_tid, NULL, pInterface, NULL );
  pthread_create ( &comm_tid, NULL, pCommSystem, NULL );
  pthread_create ( &audio_tid, NULL, pAudioSystem, NULL );
  pthread_create ( &control_tid, NULL, pControlSystem, NULL );
  pthread_create ( &environment_tid, NULL, pEnvironment, NULL );
  

  /* Wait for threads to finish */
  pthread_join ( interface_tid, NULL );
  pthread_join ( comm_tid, NULL );
  pthread_join ( audio_tid, NULL );
  pthread_join ( control_tid, NULL );
  pthread_join ( environment_tid, NULL );

  /* Destroy queues */
  destroyQueues ();

  return ( 0 );
}

static void *pInterface ( void *arg ){
  
  char  line [100]; 
  int turn;
  INTER_STATES      state,
                    state_next;
  msg_t             InMsg,
	                OutMsg;

  state_next = InterIdleOff;
  printf ( "To turn on the system press (1)...\n\n" );         
  fflush ( stdout );
  fflush ( stdin );
  fgets(line, sizeof(line), stdin);
  sscanf ( line, "%d", &turn );
  
  OutMsg.signal = (int) InterIdleOn;
  sendMessage ( &(queue [INTERFACE_Q]), OutMsg );
  for ( ; ; ){
	sleep(1);
    state = state_next;
    InMsg = receiveMessage( &(queue [INTERFACE_Q]) );
	
    switch ( state )
    {
      case InterIdleOff:
		
  		printf ( "Signal: InterIdleOn -> INTERFACE\n" );
		
        OutMsg.signal = (int) sStartCtrl;
		sendMessage ( &(queue [CONTROL_Q]), OutMsg );
		printf ( "Signal: sStartCtrl -> CONTROL\n" );  
	
        OutMsg.signal = (int) sStartAudio;
		sendMessage ( &(queue [AUDIO_Q]), OutMsg );
		printf ( "Signal: sStartAudio -> AUDIO\n" );	
	
        OutMsg.signal = (int) sStartComm;
        sendMessage ( &(queue [COMM_Q]), OutMsg );
		printf ( "Signal: sStartComm -> COMM\n" );

		OutMsg.signal = (int) sStartEnv;
        sendMessage ( &(queue [ENV_Q]), OutMsg );
		printf ( "Signal: sStartEnv -> ENV\n\n" );
		  
        state_next = InterIdleOn;
        break;
      case InterIdleOn:
        switch ( InMsg.signal )
        {
          case sConf:
            OutMsg.signal = (int) sConfCtrl;
            sendMessage ( &(queue [CONTROL_Q]), OutMsg);
		    printf ( "Signal: sConfCtrl -> CONTROL\n" );
            state_next = Display;
            break;
          case sTurnOff:
            OutMsg.signal = (int) sStopCtrl;
			sendMessage ( &(queue [CONTROL_Q]), OutMsg );
		  	printf ( "Signal: sStopCtrl -> CONTROL\n" );
			  
	        OutMsg.signal = (int) sStopAudio;
			sendMessage ( &(queue [AUDIO_Q]), OutMsg );
			printf ( "Signal: sStopAudio -> AUDIO\n" );
			  
	        OutMsg.signal = (int) sStopComm;
	        sendMessage ( &(queue [COMM_Q]), OutMsg );
			printf ( "Signal: sStopComm -> COMM\n\n" );

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
			printf ( "Signal: sNewConfData -> CONTROL\n\n" );
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
			char signal[] = "Signal: sConfData -> INTERFACE";
			printf ( "%60s\n\n", signal );
            state_next = WaitingNewConf;
            break;
          case sInterval:
            if (InMsg.decibels > maxNoise){
              
              OutMsg.signal = (int) sAlert;
              OutMsg.decibels = InMsg.decibels;
              OutMsg.contact = InMsg.contact;
              OutMsg.date1 = InMsg.date1;
              OutMsg.date2 = InMsg.date2;

              sendMessage ( &(queue [COMM_Q]), OutMsg );
			  char signal[] = "Signal: sAlert-> COMM";
			  printf ("%60s\n\n", signal );
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

            sendMessage ( &(queue [AUDIO_Q]), OutMsg );
			char signal[] = "Signal: sNewTime -> AUDIO";
			printf ( "%60s\n\n", signal );
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

  state_next = AudioOff;

  for ( ; ; ){
	sleep(1);
    state = state_next;
    InMsg = receiveMessage( &(queue [AUDIO_Q]) );
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

            sendMessage ( &(queue [CONTROL_Q]), OutMsg );
			char signal[] = "Signal: sInterval -> CONTROL";
			printf ( "%90s\n\n", signal );
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
			printf ( "Violation to the decibel limit.\n");
            printf ( "Decibels: %d \n", InMsg.decibels);         
			printf ( "Date 1: %d\n", InMsg.date1 );
			printf ( "Date 2: %d\n\n", InMsg.date2 );
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
	
  InMsg = receiveMessage( &(queue [ENV_Q]) );
  if (InMsg.signal == sStartEnv){
	  for ( ; ; ){
		  sleep(3);
		  OutMsg.signal = (int) sSound;
		  OutMsg.newSound = (int) (rand() % (90 + 1 - 1) + 1);
		  sendMessage ( &(queue [AUDIO_Q]), OutMsg );
		  printf ( "Signal: sSound -> AUDIO\n" );
		  printf ( "Decibels: %d\n", OutMsg.newSound );
	  }  
  }
  return ( NULL );
}