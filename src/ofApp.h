#pragma once

#include "ofMain.h"

#include "ofxFFTLive.h"
#include "ofxStk.h"
#include "ofxParticles.h"


 
using namespace stk;

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void audioIn(float * input, int bufferSize, int nChannels);

		void death_star();
		void set_ring_color(int);

		void fftlive_setup();
		void fftlive_update();

		void initialize_granular_synth();

		void audioOut(float * output, int bufferSize, int nChannels);

    private:

		ofCylinderPrimitive cigarette;
		ofSpherePrimitive small_sphere,big_sphere;
		ofImage texture,cigar,earth,moon;
		ofTexture image_texture;


		float cylinder_rotation_angle_Y=0;
		float cylinder_rotation_angle_Z=0;
		float ball_rotation_angle = 0;
		float death_star_angle_X = 0;
		float death_star_angle_Y = 0;
		float death_star_angle_Z = 0;

		float Rad = 400;	//The sphere's radius
		float rad = 25;


		float signal_rot_x = 0;
		float signal_rot_y = 0;
		float signal_rot_z = 90;
		float waveform_amp = 400;
		float waveform_width = 1200;
		float big_radius = 250;

		float smoothedVol = 0;
		float scaledVol = 0;
		float*fftSmoothed;

		float audioThreshold = 1;
		float audioPeakDecay = 0.91;
		float audioMaxDecay = 0.995;

		vector <float> left;
		vector <float> right;

		int buffersize = 512;
		int bufferCounter = 0;
		int buffer_history = 50;
		int last_point_x = 0;
		int last_point_y = 0;
		int rings_radius = 20;
		int last_ring_x = 0, last_ring_y = 0,last_ring_z = 0;
		int new_ring_x, new_ring_y, new_ring_z;

		int no_of_rings = 7;

		int grain_param_control = 2;
		int no_of_voices = 30;
		int grain_length = 60;
		int stretch_factor = 20;

		vector< vector<float> > leftHistory;
		// vector< vector<float> > rightHistory;

		ofEasyCam cam; // add mouse controls for camera movement

		// ofxAssimpModelLoader satellite;
		int nBandsToGet;
		ofSoundPlayer my_beats;
		ofSoundStream sound_stream;

		int vol_scale = 6;

		vector<ofPoint> vertices;
		vector<ofColor> colors;
		int nTri;		//The number of triangles
		int nVert;	//The number of the vertices equals nTri * 3

		ofMesh meshOriginal;
		ofMesh meshWarped;

		ofxFFTLive fftLive;
		ofxParticleSystem particleSystem;
		ofxParticleEmitter fire_emitter;
		

		stk::Granulate grain_synth;
		bool activate_grains = false;

	
};
