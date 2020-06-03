#include "plugin.hpp"

struct QuantumLaunch : Module {
	enum ParamIds {
		ENUMS(TRIG_PARAM, 8),
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(IN_INPUT, 8),
		ENUMS(CLK_INPUT, 8),
		ENUMS(TRIG_INPUT, 8),
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(OUT_OUTPUT, 8),
		ENUMS(FDBK_OUTPUT, 8),
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(TRIG_LIGHT, 8),
		NUM_LIGHTS
	};

bool req[8];
bool active[8];
dsp::BooleanTrigger clk[8];
dsp::BooleanTrigger button_trig[8];
dsp::BooleanTrigger cv_trig[8];
int beat[8];
int timer[8];
int last_time[8];
float pend_bright[8];
bool hold_active[8];

	QuantumLaunch() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int i = 0; i < 8; i++) {
			configParam(TRIG_PARAM + i, 0.0, 1.0, 0.0, string::f("Ch %d mute", i + 1));
		}
		onReset();		
	}

	void process(const ProcessArgs& args) override {
		const float zero[8] = {};
		float out[8] = {};
		int channels = 1;

		for (int i = 0; i < 8; i++) {

			//Get Input Voltage
			if (inputs[IN_INPUT + i].isConnected()) {
				channels = inputs[IN_INPUT + i].getChannels();
				inputs[IN_INPUT + i].readVoltages(out);
			} else if (inputs[CLK_INPUT + i].isConnected()){
				inputs[CLK_INPUT + i].readVoltages(out);
			}

			if (clk[i].process(inputs[CLK_INPUT+i].getVoltage() > 0.f)){

				if (beat[i] < 4) {
					beat[i]++;
				} else {
					beat[i] = 1;
				}	
				
				last_time[i] = timer[i];
				timer[i] = 0;
				pend_bright[i] =  1.f;
				
			}

			pend_bright[i] = pend_bright[i] - ((1.f/last_time[i]) * (1.2f));
			timer[i]++;

			//Start and Stop Request
			if ((button_trig[i].process(params[TRIG_PARAM + i].getValue() > 0.f)) || ((inputs[TRIG_INPUT + i].isConnected()) && (cv_trig[i].process(inputs[TRIG_INPUT+i].getVoltage() > 0.f)))) {
				req[i] ^= true;
				hold_active[i] = (beat[i] == 1) ? true : false;
			}

			//Set Active
			if (req[i] && (beat[i] == 1) && !(hold_active[i])) {
				active[i] ^= true;
				req[i] = false;
			}

			//Reset hold active
			if (hold_active[i] && (beat[i] != 1)) {
				hold_active[i] = false;
			}

			//Lights & Feedback
			if (active[i] && !(req[i])) {
				outputs[FDBK_OUTPUT + i].setVoltage(10.f);
				lights[TRIG_LIGHT + i].setBrightness(1.f);
			} else if (req[i]) {
				lights[TRIG_LIGHT + i].setBrightness(pend_bright[i]);
				(timer[i] < (last_time[i] / 2)) ? outputs[FDBK_OUTPUT + i].setVoltage(10.f) : outputs[FDBK_OUTPUT + i].setVoltage(0.f);
			} else {
				lights[TRIG_LIGHT + i].setBrightness(0.f);
				outputs[FDBK_OUTPUT + i].setVoltage(0.f);
			}

			// Set output
			if (outputs[OUT_OUTPUT + i].isConnected()) {
				outputs[OUT_OUTPUT + i].setChannels(channels);
				outputs[OUT_OUTPUT + i].writeVoltages(active[i] ? out : zero);
			}

		}
	}

	void onReset() override {
		for (int i = 0; i < 8; i++) {
			req[i] = false;
			active[i] = false;
			beat[i] =  1;
		}
	}
};


struct QuantumLaunchWidget : ModuleWidget {
	QuantumLaunchWidget(QuantumLaunch* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/QuantumLaunch.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		float column[8] = {22.49, 31.008, 39.526, 48.044, 56.562, 65.08, 73.598, 82.116};
		float row[6] = {30.933, 45.163, 59.393, 73.623, 87.743, 101.973};

		for (int i = 0; i < 8; i++){
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(column[i], row[0])), module, QuantumLaunch::IN_INPUT+i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(column[i], row[1])), module, QuantumLaunch::CLK_INPUT+i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(column[i], row[2])), module, QuantumLaunch::TRIG_INPUT+i));
			addParam(createParamCentered<LEDBezel>(mm2px(Vec(column[i], row[3])), module, QuantumLaunch::TRIG_PARAM+i));
			addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(column[i], row[3])), module, QuantumLaunch::TRIG_LIGHT+i));
			addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(column[i], row[4])), module, QuantumLaunch::FDBK_OUTPUT+i));
			addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(column[i], row[5])), module, QuantumLaunch::OUT_OUTPUT+i));
		}
	}
};


Model* modelQuantumLaunch = createModel<QuantumLaunch, QuantumLaunchWidget>("QuantumLaunch");
