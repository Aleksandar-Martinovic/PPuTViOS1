#include "graphics.h"
#include "vezba_5.h"


void graphicInterface()
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
            primary->DrawString(primary, "You are now listening Radio...", 30, 60, 80, DSTF_CENTER);//////////////
        }
        else
        {
            primary->SetColor(primary, 0x00, 0x00, 0x99, 0x99);
            primary->FillRectangle(primary, 0, 0, screenWidth, 100);
        }

        sprintf(channelBuffer, "Ch: %d ---> VideoPid: %d ---> AudioPid: %d", channelInfo.programNumber, channelInfo.videoPid, channelInfo.audioPid);

        printf("OVO NIJE NEOPHODNO: %s\n", channelBuffer);/////////

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
                primary->DrawString(primary, "You are now listening Radio...", 30, 60, 80, DSTF_CENTER);//////////////
            
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

}