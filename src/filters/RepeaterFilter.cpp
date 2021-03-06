
#include "RepeaterFilter.h"

namespace ofxPm{

    //------------------------------------------------------------------
    RepeaterFilter::RepeaterFilter():
        ofxOceanodeNodeModelLocalPreset("Repeater FX")
    {
        color = ofColor::orange;
        setupNodeBased();
    }
    //------------------------------------------------------------------
    RepeaterFilter::~RepeaterFilter()
    {
    }
    //------------------------------------------------------------------
    void RepeaterFilter::setupNodeBased()
    {
        color = ofColor::darkMagenta;

        parameters->add(paramFrameIn.set("Frame In", myFrame));
        addParameterToGroupAndInfo(paramDoLoop.set("Loop", false)).isSavePreset = false;
        parameters->add(paramRestart.set("Restart"));
        parameters->add(paramCapturedTimeBeatDiv.set("Time Div",1,1,32));
        parameters->add(paramCapturedTimeBeatMult.set("Time Mult",1,1,32));
//        parameters->add(paramGatePct.set("Gate",0.0,0.0,1.0));
        addParameterToGroupAndInfo(paramGatePct.set("Gate",0.0,0.0,1.0)).isSavePreset = false;
        parameters->add(paramRefreshLoopAt.set("Refresh At",4,0,32));
        parameters->add(paramSpeedBoost.set("Speed Boost",1.0,-4.0,4.0));
        
        parameters->add(paramFrameOut.set("Frame Output", myFrame));

        int i=1;
        loopTimeChanged(i);
        loopStartedAtMs=0.0;
        loopDurationMsWhenTriggered=0.0;
        BPMfactor=1.0;
        restart=false;
        oldDoLoop=false;
        phasorNumCycles=0;
        
        
        listeners.push(paramRestart.newListener(this,&RepeaterFilter::doRestart));
        listeners.push(paramDoLoop.newListener(this, &RepeaterFilter::doLoopChanged));
        listeners.push(paramCapturedTimeBeatMult.newListener(this, &RepeaterFilter::loopTimeChanged));
        listeners.push(paramCapturedTimeBeatDiv.newListener(this, &RepeaterFilter::loopTimeChanged));
        listeners.push(paramFrameIn.newListener(this, &RepeaterFilter::newVideoFrame));
        
        listeners.push(_phasor.phasorCycle.newListener(this, &RepeaterFilter::phasorCycleEvent));
        
        buffer.setupNodeBased();
        videoHeader.setup(&buffer);
        //cout << "Repeater : setting up buffer !! "<< buffer.getMaxSize() << " __ " << buffer.getSizeInFrames()  << endl;
        buffer.resume();
        
    
        
    }
    //--------------------------------------------------------
    VideoFrame RepeaterFilter::getNextVideoFrame()
    {
        return myFrame;
    }

    //--------------------------------------------------------
    void RepeaterFilter::newVideoFrame(VideoFrame & _frame)
    {
        
        bool frameIsNull = _frame.isNull();
        int buffNumFrames = buffer.getSizeInFrames();
        ofxPm::VideoFrame vfAux;
        float phase=0.0;
        
        if(buffNumFrames==0)
        {
            cout << "LooperFilter::Buffer is 0 frame long..." << endl;
        }
//        else
//        {
//            videoHeader.setup(&buffer);
//            cout << "Repeater : setting up buffer !! " << endl;
//        }
        
        if(!frameIsNull)
        {
            double elapsedSincePressedLoop = ofGetElapsedTimeMillis()-loopStartedAtMs;
            //cout << "LooperFilter:: elapsedSincrePressedLoop : " << elapsedSincePressedLoop << " // Loop Started : " << loopStarted << " // Loop Duration " << loopDurationMs << endl;
            
            float timeOffsetOnLoop = 0.0;
            
            if(!paramDoLoop)
            {
                // NO LOOP
                // not looping, so just feeding the buffer with the incoming frame.
                buffer.newVideoFrame(_frame);
                vfAux = _frame;
            }
            else if((paramDoLoop && (elapsedSincePressedLoop < loopDurationMs)))
            {
                // RECORDING LOOP
                // We've pressed "loop" , so capturing into buffer enable and we need to let the time of loop pass before stoping the capture
                buffer.newVideoFrame(_frame);
                vfAux = _frame;
                //cout << "Phasor during record : " << _phasor.getPhasor() << endl;
                timeOffsetOnLoop = elapsedSincePressedLoop - loopDurationMs;
            }
            else
            {
                // PLAYING LOOP
                // Loop already captured, relooping
                timeOffsetOnLoop = elapsedSincePressedLoop - loopDurationMs;
                phase = 1.0-fmod(_phasor.getPhasor()*paramSpeedBoost,1.0);
                //phase = 1.0 - _phasor.getPhasor();
                float loopDelayMs = phase * loopDurationMs;
                
                videoHeader.setDelayMs(loopDelayMs);
                
                float loopStart = loopDurationMsWhenTriggered;
                float loopEnd = loopDurationMsWhenTriggered - loopDurationMs;
                float whichPctOfLoopDone = ofMap(loopDelayMs,loopStart,loopEnd,0.0,1.0);

                vfAux = videoHeader.getNextVideoFrame();

            }
            
            // Gate management
            //if(whichPctOfLoopDone > (1.0 - paramGatePct))
            float ph = _phasor.getPhasor();
            if(paramDoLoop && ((ph)> (1.0 - paramGatePct)))
            {
                // we need to set to black, we're GATING !!
//                ofxPm::VideoFrame vfBlack;
                //paramFrameOut = myFrame;
            }
            else
            {
                paramFrameOut = vfAux;
            }
        }
        
    }

    //-----------------------------------------
    void RepeaterFilter::loopTimeChanged(int &_i)
    {

        _phasor.setBeatsDiv(paramCapturedTimeBeatDiv);
        _phasor.setBeatsMult(paramCapturedTimeBeatMult);
        
        cout << paramCapturedTimeBeatDiv << endl;
        if(paramCapturedTimeBeatDiv!=0)
        {
            BPMfactor = (float(paramCapturedTimeBeatMult)/float(paramCapturedTimeBeatDiv));
        }
        else  BPMfactor = 1.0;
        
        float oneBeatMs = (60.0/myBPM)*1000.0;
        loopDurationMs = oneBeatMs / BPMfactor;
        //cout << "RepeaterFilter:: Changed Loop Time Duration = " << loopDurationMs << " Ms !! BPM : " <<  myBPM << endl;
    }
    //-----------------------------------------
    void RepeaterFilter::doLoopChanged(bool& _b)
    {
//        if(paramDoLoop!=oldDoLoop)
        
        {
            int i=0;
            loopTimeChanged(i);
            loopDurationMsWhenTriggered = loopDurationMs;
            _phasor.resetPhasor();
            //cout << "LooperFilter::PRESSED LOOP !! " << endl;
            loopStartedAtMs = ofGetElapsedTimeMillis();

        }
        
        oldDoLoop=paramDoLoop;
        
        
    }
    //-----------------------------------------
    void RepeaterFilter::doRestart()
    {
        cout << "LooperFilter::Restarting!!" << endl;
        restart = true;
        _phasor.resetPhasor();
    }
    //-----------------------------------------
    void RepeaterFilter::phasorCycleEvent()
    {
        if(paramDoLoop) phasorNumCycles = phasorNumCycles +1;
        else phasorNumCycles = 0;

        //cout << "Phasor Cycle ! : "  << phasorNumCycles << endl;

        if((phasorNumCycles>=paramRefreshLoopAt)&&(paramRefreshLoopAt>0))
        {
            paramDoLoop = true;
            bool b = true;
            doLoopChanged(b);
            phasorNumCycles = 0;
        }
    }
    

    
}


