
#include "VideoGrabberPS3EyeNodeBased.h"

namespace ofxPm{
    
    //-----------------------------------------------------------------------------------
    VideoGrabberPS3EyeNodeBased::VideoGrabberPS3EyeNodeBased(): ofxOceanodeNodeModel("Video Grabber PS3 Eye")
    {
        color = ofColor::darkGreen;

        parameters->add(paramConnect.set("Connect"));
        parameters->add(paramExposure.set("Exposure",255,0,255));
        parameters->add(paramHue.set("Hue",127,0,255));
        parameters->add(paramAutoGain.set("AutoGrain",false));
        parameters->add(paramAutoWB.set("AutoWB",false));
        parameters->add(paramFrameOut.set("Frame Output", frame));
        
        listeners.push(paramConnect.newListener(this, &VideoGrabberPS3EyeNodeBased::connectToDevice));

        listeners.push(paramExposure.newListener(this, &VideoGrabberPS3EyeNodeBased::setExposure));
        listeners.push(paramAutoGain.newListener(this, &VideoGrabberPS3EyeNodeBased::setAutoGain));
        listeners.push(paramHue.newListener(this, &VideoGrabberPS3EyeNodeBased::setHue));
        listeners.push(paramAutoWB.newListener(this,&VideoGrabberPS3EyeNodeBased::setAutoWhiteBalance));

    }

    //-----------------------------------------------------------------------------------
    VideoGrabberPS3EyeNodeBased::~VideoGrabberPS3EyeNodeBased()
    {
        cout << "VideoGrabberPS3EyeNodeBased::Destroying Grabber PS3Eye..." << endl;
        if(eye)
        {
            eye->stop();
        }
    }

    //-----------------------------------------------------------------------------------
    bool VideoGrabberPS3EyeNodeBased::initGrabber(int w, int h)
    {
        
        // list out the devices
        std::vector<ps3eye::PS3EYECam::PS3EYERef> devices( ps3eye::PS3EYECam::getDevices() );
        cout << "Devices List : " << devices.size() << endl;
        
        if(devices.size())
        {
            eye = devices.at(0);
            bool res = eye->init(w, h, 60);
            eye->start();
            width = eye->getWidth();
            height = eye->getHeight();
            
            videoFrame 	= new unsigned char[eye->getWidth()*eye->getHeight()*3];
            //        videoTexture.allocate(eye->getWidth(), eye->getHeight(), GL_BGR_EXT);
            videoTexture.allocate(eye->getWidth(), eye->getHeight(), GL_RGB);
        }
        
        int expo = paramExposure.get();
        setExposure(expo);
        int wb = paramHue.get();
        setHue(wb);

        return true;
    }

    //-----------------------------------------------------------------------------------
    VideoFrame VideoGrabberPS3EyeNodeBased::getNextVideoFrame()
    {
        eye->getFrame(videoFrame);
        videoTexture.loadData(videoFrame, eye->getWidth(),eye->getHeight(), GL_BGR_EXT);
        ofPixels p;
        p.setFromPixels(videoFrame, 640, 480,OF_PIXELS_BGR);
        newFrame(p);

        return frame;
    }

    //-----------------------------------------------------------------------------------
    void VideoGrabberPS3EyeNodeBased::update(ofEventArgs &e)
    {
        if((eye!=nullptr)&&(eye->isStreaming()))
        {
            eye->getFrame(videoFrame);
            videoTexture.loadData(videoFrame, eye->getWidth(),eye->getHeight(), GL_BGR_EXT);
            ofPixels p;
            p.setFromPixels(videoFrame, 640, 480,OF_PIXELS_BGR);
            
            newFrame(p);
        }
    }

    //-----------------------------------------------------------------------------------
    void VideoGrabberPS3EyeNodeBased::newFrame(ofPixels & pixels)
    {
        frame = VideoFrame::newVideoFrame(pixels);
        paramFrameOut = frame;
    }

    //------------------------------------------------------
    float VideoGrabberPS3EyeNodeBased::getFps()
    {
        return fps;
    }
	
    //------------------------------------------------------
    void VideoGrabberPS3EyeNodeBased::setFps(float fps)
    {
        this->fps = fps;
    }

    //------------------------------------------------------
    void VideoGrabberPS3EyeNodeBased::connectToDevice()
    {
        cout << "VideoGrabberPs3EyeNodeBased::set Reconnect .... " << endl;
        
        initGrabber(640,480);
//        ofAddListener(ofEvents().update, this, &VideoGrabberPS3EyeNodeBased::update);
        
        VideoSource::setWidth(640);
        VideoSource::setHeight(480);

        cout << "VideoGrabberPS3NodeBased::reconnecting to grabber device "  << " at : " << 640 << " , " << 480 << " || FPS : " << 60 << endl;
    }

    
}
