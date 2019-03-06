#include "graphics.h"

void* graphicInterface()
{
    struct timeval now;
    struct timespec endGraph;

    pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

    gettimeofday(&now, NULL);
    endGraph.tv_sec = now.tv_sec + 3;

    if(getChannelInfo(&channelInfo) == SC_NO_ERROR)
    {
        if(channelInfo.videoPid == -1)
        {
            primary->SetColor(primary, 0x00, 0x00, 0x66, 0xff);
            primary->FillRectangle(primary, 0, 0, screenWidth, screenHeight);
            primary->SetColor(primary, 0xcc, 0xff, 0xcc, 0xff);
            primary->DrawString(primary, "You are now listening Radio...", 30, 400, 200, DSTF_CENTER);//////////////
        }
        else
        {
            primary->SetColor(primary, 0x00, 0x00, 0x6b, 0x99);
            primary->FillRectangle(primary, 0, 0, screenWidth, 100);
            primary->SetColor(primary, 0xcc, 0xcc, 0xe1, 0xff);
        }

        if(teleText == false)
        {
            sprintf(channelBuffer, "Ch: %d ---> VideoPid: %d ---> AudioPid: %d ---> TTX: NO", channelInfo.programNumber, channelInfo.videoPid, channelInfo.audioPid);
        }
        else
        {
            sprintf(channelBuffer, "Ch: %d ---> VideoPid: %d ---> AudioPid: %d ---> TTX: YES", channelInfo.programNumber, channelInfo.videoPid, channelInfo.audioPid);
        }

        primary->DrawString(primary, channelBuffer, -1, 50, 50, DSTF_LEFT);
        sleep(1);
        primary->Flip(primary, NULL, 0);
        sleep(1);

        pthread_mutex_lock(&statusMutex);
        if(ETIMEDOUT == pthread_cond_timedwait(&statusCondition, &statusMutex, &endGraph))
        {
            if(channelInfo.videoPid != -1)
            {
                primary->Clear(primary, 0, 0, 0, 0);
                sleep(1);
                primary->Flip(primary, NULL, 0);
                sleep(1);
            }
            else
            {
                primary->Clear(primary, 0, 0, 0, 0);
                primary->SetColor(primary, 0x00, 0x00, 0x66, 0xff);
                primary->FillRectangle(primary, 0, 0, screenWidth, screenHeight);
                primary->SetColor(primary, 0xcc, 0xff, 0xcc, 0xff);
                primary->DrawString(primary, "You are now listening Radio...", 30, 400, 200, DSTF_CENTER);
            
                sleep(1);
                primary->Flip(primary, NULL, 0);
                sleep(1);
            }

            pthread_mutex_unlock(&statusMutex);
            
        }
        else
        {
            printf("Info error!\n");
        }
        
        primary->Clear(primary, 0, 0, 0, 0);
    }

    return (void*)NO_ERROR;

}

void wipeScreen()
{
    printf("WIPE SCREEN\n");
    int32_t ret;

    if(currentChannel.videoPid != -1)
    {
        DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x66, 0x00));
        DFBCHECK(primary->FillRectangle(primary, 0, 0, screenWidth, 100));
        DFBCHECK(primary->Flip(primary, NULL, 0));
    }
    else
    {
        DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x66, 0xff));
        DFBCHECK(primary->FillRectangle(primary, 0, 0, screenWidth, 100));
        DFBCHECK(primary->Flip(primary, NULL, 0));
    }

    memset(&timerSpec, 0, sizeof(timerSpec));
    ret = timer_settime(timerId, 0, &timerSpec, &timerSpecOld);
    if(ret == -1)
    {
        printf("Error setting timer in %s!\n", __FUNCTION__);
    } 
    
}