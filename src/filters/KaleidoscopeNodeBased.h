
#ifndef KALEIDOSCOPENODEBASED_H_
#define KALEIDOSCOPENODEBASED_H_

#include "VideoFilter.h"
#include "ofxOceanodeNodeModel.h"

namespace ofxPm{
    
class kaleidoscopeNodeBased: public VideoFilter,public ofxOceanodeNodeModel {
public:
	kaleidoscopeNodeBased();
	virtual ~kaleidoscopeNodeBased();

    void setupNodeBased();
    void update(ofEventArgs &e) override;

    
	VideoFrame getNextVideoFrame() { return VideoFrame(); };
	void newVideoFrame(VideoFrame & frame);
    float getFps(){return fps;};


private:
    ofFbo           fbo;
	ofShader        shader;
	bool            newFrame;
    ofPlanePrimitive plane;

    float fps;
    
    // FEATURE NODE
    ofParameter<int>                    paramSides;
    ofParameter<float>                  paramAngle;
    ofParameter<float>                  paramRotation;
    ofParameter<float>                  paramSlideX;
    ofParameter<float>                  paramSlideY;
    
    ofEventListener listener;
};
}

#endif /* KALEIDOSCOPENODEBASED_H_ */
