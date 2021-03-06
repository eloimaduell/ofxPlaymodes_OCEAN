

#ifndef LOOPERFILTER_H
#define LOOPERFILTER_H

#include "VideoFilter.h"
#include "VideoBufferNodeBased.h"
#include "VideoHeaderNodeBased.h"
#include "ofxOceanodeNodeModelLocalPreset.h"
#include "basePhasor.h"

#include "ofMain.h"

namespace ofxPm{
        
class LooperFilter: public VideoFilter,public ofxOceanodeNodeModelLocalPreset
{
public:
    LooperFilter();
    virtual ~LooperFilter();
    
    void        setupNodeBased();
    VideoFrame  getNextVideoFrame();
    void        newVideoFrame(VideoFrame & _frame);
    float       getFps(){return fps;};
    void        update(ofEventArgs &e) override;

protected:
    
    float                               fps;
    ofParameter<bool>                   paramDoLoop;
    ofParameter<bool>                   paramDoRec;
    ofParameter<int>                    paramCapturedTimeBeatDiv;
    ofParameter<int>                    paramCapturedTimeBeatMult;
    ofParameter<float>                  paramGatePct;
    ofParameter<void>                   paramRestart;
    ofParameter<float>                  paramSpeedBoost;
    ofParameter<float>                  paramOffsetMs;
    ofParameter<int>                    paramBufferSize;
    ofParameter<bool>                   paramShowOnRec;
    ofParameter<int>                    paramRefreshLoopAt;
    ofParameter<bool>                   paramDoPlay;
    ofParameter<bool>                   paramDoRefresh;
    ofParameter<float>                  paramPhasorOut;
    
    ofxPm::VideoBufferNodeBased                 buffer;
    ofxPm::VideoHeaderNodeBased                 videoHeader;
    bool                                        restart;

    void                                loopTimeChanged(int& _i);
    void                                doLoopChanged(bool& _b);
    void                                doRecChanged(bool& _b);
    void                                doRestart();
    void                                bufferSizeChanged(int &i);
    void                                refreshAtChanged(int &i);
    void                                doRefreshChanged(bool &b);
    void                                playChanged(bool &b);
    
    // when global BPM changes, it will call this function so I can know the global bpm
    void                                setBpm(float _bpm) override;


private:

    // rendering
    VideoFrame                          myFrame;
    double                              loopDurationMs;
    double                              BPMfactor;
    float                               myBPM;
    
    basePhasor                          _phasor;
    
    void                                phasorCycleEvent();
    int                                 phasorNumCycles;
    
    ofEventListeners listeners;
};
}

#endif // LOOPERFILTER_H
