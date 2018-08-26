#include "EdgeBlendNodeBased.h"

namespace ofxPm{
    
    //--------------------------------------------------------------
    EdgeBlendNodeBased::EdgeBlendNodeBased():ofxOceanodeNodeModel("Edge Blend FX")
    {
        setupNodeBased();
    }

    //--------------------------------------------------------------
    EdgeBlendNodeBased::~EdgeBlendNodeBased()
    {
        fbo.clear();
        shader.unload();
    }

    //--------------------------------------------------------------
    void EdgeBlendNodeBased::setupNodeBased()
    {
        color = ofColor::darkMagenta;
        fps = -1;
        
        string shaderName = "shaders/gradient";
        shader.load(shaderName);
        cout << "EdgeBlendNodeBased::Loading Shader : " << shaderName << endl;
        
        parameters->add(paramFrameIn.set("Frame Input", frame));
        parameters->add(paramGradientWidth.set("Width",0.25,0.0,1.0));
        parameters->add(paramGradientXorY.set("X or Y",1,0,1));
        parameters->add(paramFrameOut.set("Frame Output", frame));
        
        paramGradientWidth.addListener(this, &EdgeBlendNodeBased::setGradientWidth);
        paramGradientXorY.addListener(this, &EdgeBlendNodeBased::setGradientXorY);
        paramFrameIn.addListener(this, &EdgeBlendNodeBased::newVideoFrame);
        
    }
    //------------------------------------------------------------
    void EdgeBlendNodeBased::update(ofEventArgs &e)
    {
//        if(fboHasToBeAllocated != glm::vec2(-1, -1))
//        {
//            fbo.allocate(fboHasToBeAllocated.x, fboHasToBeAllocated.y);
//            fboHasToBeAllocated = glm::vec2(-1, -1);
//        }
    }
    
    //------------------------------------------------------------
    VideoFrame EdgeBlendNodeBased::getNextVideoFrame()
    {
        return frame;
        
    }
    //--------------------------------------------------------------
    void EdgeBlendNodeBased::newVideoFrame(VideoFrame & _frame)
    {
        bool isAllocated = fbo.isAllocated();
        bool frameIsNull = _frame.isNull();
        
        if(!frameIsNull)
        {
            int w = _frame.getWidth();
            int h = _frame.getHeight();

            if (!isAllocated || _frame.getWidth() != fbo.getWidth() || _frame.getHeight() != fbo.getHeight())
            {
                fbo.allocate(w, h);
            }

            {
                fbo.begin();
                {
                    ofClear(0, 0, 0, 255);
                    shader.begin();
                    {
                        shader.setUniformTexture("tex0",_frame.getTextureRef(),11);
                        shader.setUniform1f("u_width",paramGradientWidth);
                        shader.setUniform1i("u_xory",paramGradientXorY);

                        ofSetColor(255);
                        _frame.getTextureRef().draw(0,0,w,h);
                    }
                    shader.end();
                }
                fbo.end();
                
                //frame = VideoFrame::newVideoFrame(fbo.getTexture());
                paramFrameOut = VideoFrame::newVideoFrame(fbo);
            }
        }
        
    }
}
