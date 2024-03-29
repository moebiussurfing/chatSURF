﻿#include "ofApp.h"
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setupInputText()
{
	ofLogNotice("ofApp") << "setupInputText";

	textInput.makeReferenceTo(bigTextInput.textInput);
	eTextInput = textInput.newListener([this](string& s)
		{
			doAttendCallbackTextInput();
		});

	// Optional customizations
	// Custom path for multi-instances 
	// avoid "collide folders".
	// or to organize bin/data
	// bigTextInput.setPathGlobal("Gpt");
	// bigTextInput.setName("Prompt");
	// Change the hint text:
	// bigTextInput.setHint("Type search");
	// Change the submit button text:
	//bigTextInput.setSubmit("Send");
	callback_t myFunctionCallbackClear = std::bind(&ofApp::doAttendCallbackClear, this);
	bigTextInput.setFunctionCallbackClear(myFunctionCallbackClear);

	// Extra widget to insert into the text input widget
	callback_t myFunctionDraw = std::bind(&ofApp::drawWidgetsToTextInput, this);
	bigTextInput.setDrawWidgetsFunction(myFunctionDraw);

	////TODO:
	//// Extra widget to insert into the text input widget, context
	//callback_t myFunctionDrawContext = std::bind(&ofApp::drawWidgetsContextMenu, this);
	//bigTextInput.setDrawWidgetsFunctionContextMenu(myFunctionDrawContext);

	//TODO:
	//// Keys. to trig key sounds
	//callback_t myFunctionCallbackKeys = std::bind(&ofApp::doAttendCallbackKeys, this);
	//bigTextInput.setFunctionCallbackKeys(myFunctionCallbackKeys);

	// Link toggles
	bigTextInput.bDebug.makeReferenceTo(ui.bDebug);
	////bigTextInput.bWaiting.makeReferenceTo(bWaitingGpt);
}

//--------------------------------------------------------------
void ofApp::setupImGui()
{
	bGui.set("surfChat", true);

	//ui.setLogLevel(OF_LOG_VERBOSE);
	//ui.setDisableStartupResetLayout();

	ui.setImGuiViewPort(true);
	ui.setup();

	string p;
	string s;

	/*
	p = "assets/fonts2/Montserrat-Bold.ttf";
	ui.setDefaultFont(p, 14);
	*/


	// Customize Default font with the 4 styles
	// string p = "assets/fonts2/Inter-Bold.ttf";
	//string p = "assets/fonts2/Poppins-Regular.ttf";
	//string p = "assets/fonts2/Gilmer Medium.otf";
	//string p = "assets/fonts2/Montserrat-Bold.ttf";
	//float sz = 14;
	float sz = 15;
	p = "assets/fonts2/NotoSans-Regular.ttf";
	ui.setupFontForDefaultStyles(p, sz);

	//p = "assets/fonts/JetBrainsMono-ExtraBold.ttf";
	//ui.setupFontForDefaultStyles(p, sizeFont);
	p = "assets/fonts2/NotoSansMono-Regular.ttf";
	ui.setupFontForDefaultStyles(p, sz);

	/*
	s = "assets/fonts2";
	ui.pushFontsFromFolder(s, sizeFont);
	*/
	//ui.bLoadFontsFromFolder
}

//--------------------------------------------------------------
void ofApp::setup()
{
	ofLogNotice("ofApp") << "setup";

	ofSetLogLevel(OF_LOG_VERBOSE);

	ofAddListener(ofEvents().windowResized, this, &ofApp::windowResized);

#ifdef SURFING_WINDOW_APP__USE_STATIC
	w.setup(&w);
	//w.setEnableKeys(false);
#endif
#if 1
	//w.doReset();
	//ofxSurfingHelpers::setMonitorsLayout(1, true, true);
#endif

	string s = "surfChat";
	ofSetWindowTitle(s);

	//--

	////ui.setImGuiViewPort(true);
	//ui.setup();

	//--

	// Text input
	setupInputText();

	bWaitingGpt.setSerializable(0);

	//--

	setupImGui();

	//--

#ifdef USE_SURF_SUBTITLES
	subs.setUiPtr(&ui);
	subs.setup(); // Startup with no subs mode
#endif

	//--

#ifdef USE_WHISPER
	whisper.setup();
	whisper.vCallback.addListener(this, &ofApp::doUpdatedWhisper);
#endif

	//ui.ClearLogDefaultTags();
	//ui.AddLogTag(ofColor::white);

	//--

	// Editors

#ifdef USE_EDITOR_INPUT
	editorInput.setup("Input");
	editorInput.setCustomFonts(ui.getFontsPtr(), ui.getFontsNames());

	//// custom keyword
	//editorInput.addKeyword("\"user\":");
	//editorInput.addKeyword("\"assistant\":");

	// Advanced
	callback_t myFunctionDraw = std::bind(&ofApp::drawWidgetsEditor, this);
	//std::function<void()> myFunctionDraw = std::bind(&ofApp::drawWidgetsEditor, this);
	editorInput.setDrawWidgetsFunction(myFunctionDraw);
#endif

	//--

#ifdef USE_EDITOR_RESPONSE
	editorLastResponse.setup("Last Response");
	editorLastResponse.setCustomFonts(ui.getFontsPtr(), ui.getFontsNames());
#endif

	//--

	setupParams();

	//--

	startup();
}

//--------------------------------------------------------------
void ofApp::setupParams()
{
	ofLogNotice("ofApp") << "setupParams";

	params.add(bGui);
	params.add(bigTextInput.bGui);
	params.add(ui.bGui_GameMode);
	params.add(bLock);

	bGui_WindowContextMenu.setSerializable(0);
	bGui_WindowContextMenu.set("ContextMenu", 0);
	params.add(bGui_WindowContextMenu);

	params.add(colorBg);
	params.add(colorAccent);
	params.add(colorUser);
	params.add(colorAssistant);

#ifdef USE_SURF_SUBTITLES
	params.add(subs.bGui);
#endif

	paramsConversations.add(bGui_GptConversation);
	paramsConversations.add(bModeConversation);
	paramsConversations.add(bLastBigger);
	paramsConversations.add(bLastBlink);
	paramsConversations.add(bWaitingGpt);
	paramsConversations.add(indexSize);
	paramsConversations.add(bScaleWindow);
	paramsConversations.add(sizeFontConv);
	paramsConversations.add(scaleWindow);
	params.add(paramsConversations);

	params.add(bModeOneSlide); //for subtitles

	typeSpin.setMax(ImSpinner::amountSpinners - 1);
	params.add(typeSpin);

	params.add(indexModel);

	paramsPrompts.add(indexPrompt);
	paramsPrompts.add(indexTags);
	paramsPrompts.add(amountResultsPrompt);
	params.add(paramsPrompts);

	// Layout
	params.add(bBottomTextInput);
	params.add(bigTextInput.windowY);
	params.add(bigTextInput.windowPadX);
	params.add(spacingX);
	params.add(spacingY);

	ofAddListener(params.parameterChangedE(), this, &ofApp::Changed_Params);

	// Link
	bigTextInput.bBottomTextInput.makeReferenceTo(bBottomTextInput);
	bigTextInput.colorBubble.makeReferenceTo(colorAccent);
	bigTextInput.colorTxt.makeReferenceTo(colorUser);

	// Presets
#ifdef USE_PRESETS
	paramsPreset.setName("surfChat");
	paramsPreset.add(colorBg);
	paramsPreset.add(colorAccent);
	paramsPreset.add(colorUser);
	paramsPreset.add(colorAssistant);
	paramsPreset.add(bigTextInput.paramsPreset);
	paramsPreset.add(spacingX);
	paramsPreset.add(spacingY);
	paramsPreset.add(bBottomTextInput);

	presets.setUiPtr(&ui);
	presets.AddGroup(paramsPreset);
#endif
}

//--------------------------------------------------------------
void ofApp::startup()
{
	ofLogNotice("ofApp") << "startup";

	setupGpt(0);

	//TODO:
	//bigTextInput.typeWaiting = typeSpin;

	//ui.ClearLogDefaultTags();

	// Default
	doResetAll(1);

	//ofxSurfingHelpers::load(params);
	g.addGroup(params);

	bDoneStartup = 1;
}

//--------------------------------------------------------------
void ofApp::startupDelayed()
{
	ofLogNotice("ofApp") << "startupDelayed";

	doRefreshGptPrompts();

	bDoneStartupDelayed = 1;

	// fixes
	amountResultsPrompt = amountResultsPrompt;

	//--

	// g.addGroup(params);
	//ofxSurfingHelpers::load(params);
}

//--------------------------------------------------------------
void ofApp::setupGpt(bool bSilent)
{
	ofLogNotice("ofApp") << "setupGpt";

	//if(!bSilent) sounds.play(5);

	//if (apiKey.get() == "")
	//{
	//	ui.AddToLog("No settled API key to run Setup GPT", OF_LOG_ERROR);
	//	return;
	//}

	{
		ui.AddToLog("setupGpt()", OF_LOG_WARNING);

		//--

		// GPT

		// Load file settings

		ofFile f;
		if (f.doesFileExist(pathGptSettings))
		{
			ofJson configJson = ofLoadJson(pathGptSettings);

			if (configJson.find("apiKey") != configJson.end())
				apiKey = configJson["apiKey"].get<string>();

			if (configJson.find("model") != configJson.end())
				model = configJson["model"].get<string>();
		}
		else
		{
			apiKey = "<your-api-key>"; //deafult
			model = "gpt-3.5-turbo"; //default
		}

		//--

		ofxChatGPT chatGpt_;
		chatGpt_.setup(apiKey.get());
		vector<string> models;

		ofxChatGPT::ErrorCode errorCode;
		tie(models, errorCode) = chatGpt_.getModelList();

		ui.AddToLog("Available OpenAI GPT models:");
		modelsNames.clear();
		for (auto m : models)
		{
			if (ofIsStringInString(m, "gpt"))
			{
				ui.AddToLog(m, OF_LOG_WARNING);
			}
			else
			{
				ui.AddToLog(m, OF_LOG_VERBOSE);
			}

			modelsNames.push_back(m);
		}
		indexModel.setMax(modelsNames.size() - 1);

		bWaitingGpt = 0;

		//--

		// Initialize
		chatGpt.setup(model, apiKey);
	}

	//--

	setupGptPrompts();
}

//--------------------------------------------------------------
void ofApp::doRefreshGptPrompts()
{
	ofLogNotice("ofApp") << "doRefreshGptPrompts";

	tagWord = tagsNames[indexTags.get()];
	setupGptPrompts();
	doGptSetPrompt(indexPrompt);
}

//--------------------------------------------------------------
void ofApp::setupGptPrompts()
{
	ofLogNotice("ofApp") << "setupGptPrompts";

	// Create prompts
	promptsNames.clear();

	promptsNames.push_back("Default");
	promptsNames.push_back(ofToString("Words"));
	promptsNames.push_back(ofToString("Sentences"));
	promptsNames.push_back(ofToString("Similar"));

	indexPrompt.setMax(promptsNames.size() - 1);

	ofLogNotice("ofApp") << ofToString(promptsNames);

	promptsContents.clear();
	promptsContents.push_back(doCreateGptRolePromptDefault());
	promptsContents.push_back(doCreateGptRolePromptWords());
	promptsContents.push_back(doCreateGptRolePromptSentences());
	promptsContents.push_back(doCreateGptRolePromptSimilar());
}

//--------------------------------------------------------------
void ofApp::doGptSetPrompt(int index)
{
	if (!bDoneStartupDelayed) return;

	//if (indexPrompt == index) return;//skip if not changed

	ofLogNotice("ofApp") << "doGptSetPrompt:" << index;

	sounds.play(5);

	ui.AddToLog("doGptSetPrompt(" + ofToString(index) + ")", OF_LOG_WARNING);

	if (index > promptsContents.size() - 1)
	{
		ui.AddToLog("Index " + ofToString(index) + " out of range!", OF_LOG_ERROR);
		return;
	}

	if (indexPrompt != index) indexPrompt = index;

	promptName = ofToString(promptsNames[indexPrompt]);
	strPrompt = promptsContents[indexPrompt];
	//promptName = ofToString(prompts[indexPrompt].first);
	//strPrompt = prompts[indexPrompt].second;

	// Set Role prompt
	chatGpt.setSystemMessage(strPrompt);

	// doSendSilentMessageToConversation(strPrompt);

	ui.AddToLog("Prompt: " + promptName, OF_LOG_VERBOSE);
	ui.AddToLog(strPrompt, OF_LOG_VERBOSE);
}

//--------------------------------------------------------------
void ofApp::Changed_Params(ofAbstractParameter& e)
{
	string n = e.getName();
	ofLogNotice("ofApp") << "Changed_Params: " << n << ": " << e;

	if (n == bLock.getName())
	{
		bigTextInput.bGui_LockMove = bLock;
	}

	else if (n == bGui_WindowContextMenu.getName())
	{
		// if (bGui_WindowContextMenu) ImGui::OpenPopup("my_popup");
	}

	else if (n == ui.bGui_GameMode.getName())
	{
		if (ui.bGui_GameMode) // Game mode hides many stuff!
		{
			bLock = 1;
			ui.bDebug = 0;
			ui.bLog = 0;
			ui.bNotifier = 1;
			ui.bExtra = 0;
			bigTextInput.bGui = 1;
			bigTextInput.bGui_Config = 0;
			bGui_GptConversation = 1;
			bModeConversation = 1;
			//ui.bMinimize = 1;

#ifdef USE_EDITOR_RESPONSE
			editorLastResponse.bGui = 0;
#endif

#ifdef USE_PRESETS
			presets.bGui = 0;
#endif
		}
	}
	else if (n == indexModel.getName())
	{
		if (modelsNames.size() == 0) return; //skip out of range
		if (indexModel > modelsNames.size() - 1) indexModel = modelsNames.size() - 1; //clamp
		model = modelsNames[indexModel];

		//--

		string s = "MODEL: " + model.get();
		ui.AddToLog(s, OF_LOG_WARNING);

		// Initialize
		chatGpt.setup(model.get(), apiKey);

		// workflow
		doSendSilentMessageToConversation(s);
		// doGptSendMessage(s);
	}

	else if (n == typeSpin.getName())
	{
		bigTextInput.typeWaiting = typeSpin;
	}

	else if (n == bWaitingGpt.getName())
	{
		bigTextInput.bWaiting = bWaitingGpt;
		//if (bWaitingGpt) bigTextInput.bWaiting = bWaitingGpt;
	}
	else if (n == bModeConversation.getName())
	{
		// workflow
		if (bModeConversation)
		{
#ifdef USE_SURF_SUBTITLES
			subs.bGui = 0;
			subs.bGui_List = 0;
			subs.bGui_Paragraph = 0;
#endif
		}
	}

	//--

	//TODO: workaround to avoid starting calls...
	//if (!bDoneStartupDelayed) return;

	if (n == indexSize.getName())
	{
		indexSize.setWithoutEventNotifications(ofClamp(indexSize, 0, indexSize.getMax()));
		string s = "SIZE: " + sizeNames[indexSize];
		doSendSilentMessageToConversation(s);
	}
	// Size reply for default
	if (n == indexSize.getName() && indexPrompt == 0)
	{
		setupGptPrompts();
		doGptSetPrompt(indexPrompt);

		if (textHistory.size() > 0)
		{
			string s = textHistory.back();
			doGptSendMessage(s);
		}
		return;
	}

	// Prompts
	else if (n == amountResultsPrompt.getName())
	{
		setupGptPrompts();
		doGptSetPrompt(indexPrompt);
	}
	else if (n == indexTags.getName()) //area
	{
		tagWord = tagsNames[indexTags.get()];

		setupGptPrompts();
		doGptSetPrompt(indexPrompt);

		string s = "AREA: " + tagWord;
		// string s = "AREA: " + ofToUpper(tagWord);
		doSendSilentMessageToConversation(s);
	}
	else if (n == indexPrompt.getName()) //role
	{
		doGptSetPrompt(indexPrompt);

		string s = "ROLE: " + promptName;
		// string s = "ROLE: " + ofToUpper(promptName);
		doSendSilentMessageToConversation(s);

		// workflow
		if (textHistory.size() > 0)
		{
			doGptSendMessage(textHistory.back());
		}
	}

	// refresh layout
	else if (n == bigTextInput.windowY.getName() ||
		n == bigTextInput.windowPadX.getName() ||
		n == bBottomTextInput.getName() ||
		n == spacingX.getName() ||
		n == spacingY.getName())
	{
		doRefreshWindowsLayout();
	}
}

//--------------------------------------------------------------
void ofApp::update()
{
	//ofLogNotice("ofApp") << "ofGetFrameNum:" << ofGetFrameNum();

	if (bDoneStartup)
	{
		if (!bDoneStartupDelayed)
		{
			ofLogNotice("ofApp") << "update: ofGetFrameNum:" << ofGetFrameNum();
			startupDelayed();
		}
	}

	//string s = "SurfChat | " + ofToString(ofGetFrameRate(), 0) + "FPS";
	//ofSetWindowTitle(s);

	//--

	if (chatGpt.hasMessage())
	{
		doGptGetMessage();
	}

	//--

#ifdef USE_WHISPER
	whisper.update();
#endif

	//--

	// #ifdef USE_OFX_ELEVEN_LABS
	//     if (tts.isWaiting()) bigTextInput.bWaiting = 1;
	// #endif

#ifdef USE_PRESETS
	if (presets.isChangedIndex())
	{
		doRefreshWindowsLayout();
	}
#endif

	//--

	ofSoundUpdate();
}

//--------------------------------------------------------------
void ofApp::drawBg()
{
	if (bGptError)
	{
		// Red if error.
		float v = glm::cos(10 * ofGetElapsedTimef());
		float a1 = ofMap(v, -1, 1, 100, 200, true);
		ofColor c = ofColor(a1, 0, 0);
		ofClear(c);
	}
	else if (bWaitingGpt)
	{
		// Fade blink when waiting. 
		ofColor c = colorBg.get();
		//ofColor c = bigTextInput.getColor();
		auto br = c.getBrightness();
		float g = 5; //gap
		float v = glm::cos(10 * ofGetElapsedTimef());
		float a1 = ofMap(v, -1, 1, br - g, br, true);
		//float a1 = ofMap(v, -1, 1, 100, 150, true);
		c.setBrightness(a1);
		ofClear(c);
	}
	else
	{
		// Flash when submit
		if (v > 0) v -= 0.05f;
		else v = 0;
		int bgMin = 100;
		if (v > 0) ofClear(bgMin + (255 - bgMin) * v);
		else
		{
			// standby
#ifdef USE_SURF_SUBTITLES
			// Use color from subtitler when no flash
			ofClear(subs.getColorBg());
#else
			ofClear(colorBg.get());
#endif
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// Bg
	drawBg();

#ifdef USE_SURF_SUBTITLES
	subs.draw();
#endif

	//--

	drawImGui();

	//--

#ifdef USE_WHISPER
	ofPushMatrix();
	ofTranslate(-15, ofGetHeight() * 0.7);
	whisper.draw();
	ofPopMatrix();
#endif

#ifdef USE_OFX_ELEVEN_LABS
	// if (ui.bDebug) tts.drawDebugHelp();
#endif
}

#ifdef USE_OFX_ELEVEN_LABS
//--------------------------------------------------------------
void ofApp::drawImGuiElebenLabs2()
{
	string s = "";
	if (ui.BeginTree("ElevenLabs"))
	{
		ui.AddSpacing();
		ui.Add(tts.bEnable, OFX_IM_TOGGLE);
		ui.AddTooltip("Text To Speech\n");
		if (tts.bEnable)
		{
			if (ui.isMaximized()) ui.Add(tts.bModeUseAltServer);

			if (ui.AddButton("Send", OFX_IM_BUTTON, 2, true))
			{
				tts.doSend(textLastResponse);
			}
			if (ui.AddButton("Cancel", OFX_IM_BUTTON, 2))
			{
				tts.doCancelRequest();
			}
			if (!tts.bModeUseAltServer)
			{
				ui.Add(tts.stability, OFX_IM_HSLIDER_MINI);
				ui.Add(tts.similarity_boost, OFX_IM_HSLIDER_MINI);
			}
			ui.AddSpacingSeparated();

			//center
			float ww = ImGui::GetContentRegionAvail().x;
			float ws = ImSpinner::width;
			ui.AddSpacingX((ww / 2 - ws / 2));
			ImSpinner::Spinner(tts.isWaiting(), typeSpin);
			if (ui.bDebug)
			{
				s = tts.getTextDisplayHelp();
				ui.AddLabel(s);
			}
		}

		ui.EndTree();
	}
}

//--------------------------------------------------------------
void ofApp::drawImGuiElebenLabs1()
{
	string s = "";

	//s = "ElevenLabs";
	//ui.AddLabelBig(s, 1);
	if (ui.BeginTree("ElevenLabs"))
	{
		ui.AddSpacing();
		ui.Add(tts.bEnable, OFX_IM_TOGGLE);
		ui.AddTooltip("Text To Speech\n");
		if (tts.bEnable)
		{
			if (ui.isMaximized()) ui.Add(tts.bModeUseAltServer);
			if (ui.AddButton("Send", OFX_IM_BUTTON, 2, true))
			{
				tts.doSend(textLastResponse);
			}
			s = tts.getText();
			ui.PushFontStyle(SurfingFontTypes(1));
			ui.AddTooltip(s);
			ui.PopFontStyle();

			if (ui.AddButton("Cancel", OFX_IM_BUTTON, 2))
			{
				tts.doCancelRequest();
			}

			ui.AddCombo(tts.voiceIndex, tts.voicesNames);
			ui.AddTooltip("Voices");
			ui.AddSpacing();
			if (!tts.bModeUseAltServer)
			{
				ui.Add(tts.stability, OFX_IM_HSLIDER_MINI);
				ui.Add(tts.similarity_boost, OFX_IM_HSLIDER_MINI);
			}
			ui.AddSpacingSeparated();
			if (ui.isMaximized())
			{
				drawImGuiElebenLabsExtras();
			}
			// if (ui.bDebug)
			{
				ui.AddSpacing();
				s = tts.getTextDisplayHelp();
				ui.AddLabel(s);
			}

			//center
			float ww = ImGui::GetContentRegionAvail().x;
			float ws = ImSpinner::width;
			ui.AddSpacingX((ww / 2 - ws / 2));
			ImSpinner::Spinner(tts.isWaiting(), typeSpin);
			if (ui.bDebug)
			{
				s = tts.getTextDisplayHelp();
				ui.AddLabel(s);
			}
		}

		ui.EndTree();
	}
}
#endif

//--------------------------------------------------------------
void ofApp::drawImGuiGpt1()
{
	//ui.AddLabelHuge("ChatGPT");
	if (ui.BeginTree("ChatGPT"))
	{
		ui.AddSpacing();

		if (ui.AddButton("Restart##2", bGptError ? OFX_IM_BUTTON_BORDER_BLINK : OFX_IM_BUTTON, 2, 1))
		{
			doGptRestart();
		}
		if (ui.AddButton("Clear##2", OFX_IM_BUTTON, 2))
		{
			doClear();
		}
		if (ui.AddButton("Regen##2", OFX_IM_BUTTON, 2, 1))
		{
			doGptRegenerate();
		}
		if (ui.AddButton("Resend##2", OFX_IM_BUTTON, 2))
		{
			doGptResend();
		}
		ui.AddSpacingSeparated();

		// static ofParameter<bool> b{"SERVER", 0};
		// if (ui.isMaximized())
		// {
		//     ui.AddSpacing();
		//     ui.Add(b, OFX_IM_TOGGLE_BORDER_BLINK);
		// }
		// if (ui.isMaximized() && b)
		if (ui.isMaximized())
		{
			if (ui.BeginTree("SERVER"))
			{
				ui.AddSpacing();
				ui.AddLabelBig("API KEY");
				ui.Add(apiKey, OFX_IM_TEXT_INPUT_NO_NAME);
				ui.AddLabelBig("Endpoint");
				string s = chatGpt.getEndPoint();
				ui.AddLabel(s);
				ui.AddSpacing();

				ui.AddLabelBig("MODEL");
				//ui.Add(model, OFX_IM_TEXT_DISPLAY);
				//ui.AddLabel(model.get());
				ui.AddCombo(indexModel, modelsNames);
				ui.AddSpacing();

				if (ui.AddButton("Restart", OFX_IM_BUTTON, 2))
				{
					doGptRestart();
				}
				ui.SameLine();
				if (ui.AddButton("ResetIP", OFX_IM_BUTTON, 2))
				{
					doGptResetEndpointIP();
				}
				ui.EndTree();
			}
		}
		ui.AddSpacingSeparated();

#ifdef USE_EDITOR_INPUT
		//ui.PushFontStyle(OFX_IM_FONT_BIG);
		{
			if (ui.AddButton("Send"))
			{
				doGptSendMessage(editorInput.getText(), bModeConversation);
			}
		}
		//ui.PopFontStyle();
#endif

		// ui.AddLabelBig("Prompt");
		// ui.AddSpacing();

		// //ui.AddLabel("Role Prompt", 1);
		// static ofParameter<bool> bRole{"ROLE PROMPT", 0};
		// ui.Add(bRole, OFX_IM_TOGGLE_BORDER_BLINK);
		// if (bRole)
		if (ui.BeginTree("ROLE PROMPT"))
		{
			ui.AddSpacing();
			ui.AddCombo(indexPrompt, promptsNames);
			ui.PushFontStyle(SurfingFontTypes(1));
			ui.AddTooltip(strPrompt);
			ui.PopFontStyle();
			if (indexPrompt != 0)
			{
				ui.AddCombo(indexTags, tagsNames);
				ui.Add(amountResultsPrompt, OFX_IM_STEPPER);
			}
			ui.EndTree();
		}

		//--

		if (ui.isMaximized() && bGui_GptConversation)
		{
			string s;

			ui.AddSpacingSeparated();
			// ui.Add(bModeConversation, OFX_IM_TOGGLE);
			// if (bModeConversation)
			if (ui.BeginTree("CONVERSATION"))
			{
				string s;
				ui.AddSpacing();
				if (bGui_GptConversation)
				{
					ui.Add(bLastBlink, OFX_IM_TOGGLE_ROUNDED_MINI);
					s = "Last block will blink";
					ui.AddTooltip(s);
					ui.Add(bLastBigger, OFX_IM_TOGGLE_ROUNDED_MINI);
					s = "Last block will be bigger";
					ui.AddTooltip(s);
					// ui.AddSpacing();
					// if (ui.AddButton("Reset Layout"))
					// {
					//     doResetWindowConversation();
					// }
				}
				ui.EndTree();
			}
			// ui.AddSpacingBigSeparated();

			s = gptErrorMessage;
			if (s != "")
			{
				ui.AddSpacingBigSeparated();

				//s = errorCodesNames[indexErrorCode.get()];
				//ui.AddLabelBig(s);
				ui.AddLabelBig(s);
			}
#if(1)
#if(0)//TODO:
			if (ui.bDebug)
			{
				ui.Add(bWaitingGpt, OFX_IM_CHECKBOX);
				if (ui.Add(typeSpin, OFX_IM_STEPPER))
				{
					//bigTextInput.typeWaiting = typeSpin;
				}
			}
#endif
			//center
			float ww = ImGui::GetContentRegionAvail().x;
			float ws = ImSpinner::width;
			ui.AddSpacingX((ww / 2 - ws / 2));
			ImSpinner::Spinner(bWaitingGpt, typeSpin);
			//ui.AddSpacingBigSeparated();
#endif

#ifdef USE_EDITOR_INPUT
			ui.AddLabelHuge("EDITORS");
			ui.AddSpacing();
			ui.Add(editorInput.bGui, OFX_IM_TOGGLE_ROUNDED);
#endif
		}

		ui.EndTree();
	}
}

//--------------------------------------------------------------
void ofApp::drawImGuiGpt2()
{
	if (ui.BeginTree("ChatGPT"))
	{
		ui.AddSpacing();

		if (ui.AddButton("Restart##1", bGptError ? OFX_IM_BUTTON_BORDER_BLINK : OFX_IM_BUTTON, 2, 1))
		{
			doGptRestart();
		}
		if (ui.AddButton("Clear##1", OFX_IM_BUTTON, 2))
		{
			doClear();
		}
		if (ui.AddButton("Regen##1", OFX_IM_BUTTON, 2, 1))
		{
			doGptRegenerate();
		}
		if (ui.AddButton("Resend##1", OFX_IM_BUTTON, 2))
		{
			doGptResend();
		}
		ui.AddSpacingSeparated();

		string s = "PROMPT ROLE";
		ui.AddLabel(s);
		ui.AddTooltip(strPrompt);
		ui.AddCombo(indexPrompt, promptsNames);
		// ui.PushFontStyle(SurfingFontTypes(1));
		// ui.PopFontStyle();
		if (indexPrompt != 0)
		{
			ui.AddCombo(indexTags, tagsNames);
			ui.Add(amountResultsPrompt, OFX_IM_STEPPER);
		}

		ui.EndTree();
	}
}

#ifdef USE_OFX_ELEVEN_LABS
//--------------------------------------------------------------
void ofApp::drawImGuiElebenLabsExtras()
{
	//auto drawElevenLabsExtras = [this]() {
	ui.AddSpacing();
	if (ui.AddButton("Reset##tts", OFX_IM_BUTTON))
	{
		tts.doReset();
	}
	ui.AddTooltip("Reset settings");

	if (ui.AddButton("Replay##tts", OFX_IM_BUTTON, 2, 1))
	{
		tts.doReplayAudio();
	}
	ui.AddTooltip("Replay last audio");

	if (ui.AddButton("Resend##tts", OFX_IM_BUTTON, 2))
	{
		tts.doResend();
	}
	ui.AddTooltip("Resend last text");

	if (ui.AddButton("Cancel##tts", OFX_IM_BUTTON, 2, 1))
	{
		tts.doCancelRequest();
	}
	ui.AddTooltip("Force stop request");

	if (ui.AddButton("Restart##tts", OFX_IM_BUTTON, 2))
	{
		tts.doRestart();
	}
	ui.AddTooltip("Restart connection");
	//};
}
#endif

//--------------------------------------------------------------
void ofApp::drawImGuiMain()
{
	if (!bGui) return;

	static auto drawColorTree = [this]()
	{
		if (ui.BeginTree("Colors"))
		{
			ui.Add(colorBg, OFX_IM_COLOR_BOX_FULL_WIDTH_NO_ALPHA);
			ui.Add(colorAccent, OFX_IM_COLOR_BOX_FULL_WIDTH);
			ui.Add(colorAssistant, OFX_IM_COLOR_BOX_FULL_WIDTH);
			ui.Add(colorUser, OFX_IM_COLOR_BOX_FULL_WIDTH);
			ui.EndTree();
		}
	};

	//----

#if 1
	if (bGui)
	{
		float w = 250;
		ImGui::SetNextWindowSize(ImVec2(w, 0), ImGuiCond_FirstUseEver);
		// Constraints
		ImVec2 size_min = ImVec2(w, 100);
		ImVec2 size_max = ImVec2(w, FLT_MAX);
		ImGui::SetNextWindowSizeConstraints(size_min, size_max);
	}
#endif
	if (ui.BeginWindow(bGui))
	{
		bool isFocused = ImGui::IsWindowFocused();
		bool isHovered = ImGui::IsWindowHovered();
		bIsOver_GuiMain = isFocused || isHovered;

		//TODO:
		// workaround. breaks text input focus
		// Fix focus issues when conversation window 
		// is on top of the z-order blocking other windows.
		if (0)
		{
			if (ofGetFrameNum() % 120 == 0 && !bIsInteractingWindowConversation)
			{
				ui.setWindowFocused(bGui);
				//TODO; not works bc not inside the same window. needs another approach
				// #ifdef USE_PRESETS
				//                 ui.setWindowFocused(presets.bGui);
				// #endif
			}
		}

		string s;

		ui.PushFontStyle(SurfingFontTypes(1));
		ui.Add(ui.bGui_GameMode, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
		ui.PopFontStyle();

		ui.AddSpacing();
		ui.AddMinimizerToggle();

		// No Game
		if (!ui.isGameMode())
		{
			ui.AddSpacing();
			if (ui.isMaximized())
			{
				ui.AddLogToggle();
				ui.AddNotifierToggle();
				ui.AddDebugToggle();
				//ui.AddExtraToggle();
			}
			if (ui.isMaximized())
			{
				ui.Add(bLock, OFX_IM_TOGGLE_ROUNDED_MINI);
			}

#ifdef USE_PRESETS
			ui.AddSpacing();
			ui.Add(presets.bGui, OFX_IM_TOGGLE_ROUNDED);
#endif
		}

		ui.AddSpacingBigSeparated();

		//--

		if (ui.BeginTree("UI"))
		{
			string s = "";
			ui.Add(bigTextInput.bGui, OFX_IM_TOGGLE_ROUNDED);
			if (ui.isMaximized() && bigTextInput.bGui) ui.Add(bigTextInput.bGui_Config, OFX_IM_TOGGLE_ROUNDED_SMALL);
			ui.AddSpacingSeparated();
			ui.Add(bGui_GptConversation, OFX_IM_TOGGLE_ROUNDED);
#ifdef USE_EDITOR_RESPONSE
			ui.Add(editorLastResponse.bGui, OFX_IM_TOGGLE_ROUNDED_MINI);
#endif

			ui.AddLabel("Font Size", 1);
			ui.DrawWidgetsFonts(sizeFontConv, 0);
			ui.AddSpacingSeparated();

			// ui.AddLabelBig("LAYOUT");
			if (ui.BeginTree("Layout"))
			{
				ui.AddSpacing();
				// ui.AddLabelBig("TextInput");
				ofxImGuiSurfing::AddToggleNamed(bBottomTextInput, "Bottom", "Top");
				ui.Add(bigTextInput.windowPadX, OFX_IM_HSLIDER_MINI);
				ui.Add(bigTextInput.windowY, OFX_IM_HSLIDER_MINI);
				// ui.AddLabelBig("Conversation");
				ui.Add(spacingX, OFX_IM_HSLIDER_MINI);
				ui.Add(spacingY, OFX_IM_HSLIDER_MINI);
				ui.Add(bigTextInput.rounded, OFX_IM_HSLIDER_MINI);
				ui.AddSpacing();
				if (ui.AddButton("Reset Layout"))
				{
					doResetWindowsLayout();
				}
				ui.EndTree();
			}
			ui.AddSpacingSeparated();

			// if (ui.AddButton("Top", OFX_IM_BUTTON, 2, true))
			// {
			//     bFlagGoTop = 1;
			// }
			// ui.SameLine();
			// if (ui.AddButton("Bottom", OFX_IM_BUTTON, 2))
			// {
			//     bFlagGoBottom = 1;
			// }
			// ui.AddSpacingSeparated();

			drawColorTree();

			ui.EndTree();
		}

		ui.AddSpacingSeparated();

		// Game
		if (ui.isGameMode())
		{
			//--

			drawImGuiGpt2();

#ifdef USE_OFX_ELEVEN_LABS
			ui.AddSpacingSeparated();
			drawImGuiElebenLabs2();
#endif
		}

		// No Game
		//if (!ui.isGameMode())
		else
		{
			drawImGuiGpt1();

			ui.AddSpacingSeparated();

			//--

#ifdef USE_OFX_ELEVEN_LABS
			drawImGuiElebenLabs1();
#endif
			//--

#ifdef USE_WHISPER
			drawImGuiWidgetsWhisper();
#endif

			//--

#ifdef USE_SURF_SUBTITLES
			if (!bModeConversation)
			{
				ui.AddSpacingBigSeparated();
				ui.AddLabelHuge("Titles");
				ui.AddSpacing();

				ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
				if (subs.bGui)
				{
					ui.AddSpacing();
					ui.AddSpacingDouble();
					ui.Add(bModeOneSlide, OFX_IM_CHECKBOX);

					//ui.AddLabelBig("Tester", false, true);
					static bool bTester = 0;
					ImGui::Checkbox("Tester##2", &bTester);
					//ui.PushFontStyle(OFX_IM_FONT_BIG);
					{
						if (bTester) {
							s = "Random\nText!";
							s = ofToUpper(s);
							if (ui.AddButton(s, OFX_IM_BUTTON_BIG))
							{
								doPopulateText();
							}
							s = "Do\nBlock!";
							s = ofToUpper(s);
							if (ui.AddButton(s, OFX_IM_BUTTON_BIG))
							{
								doPopulateTextBlocks();
							}
							s = "Clear\nList";
							s = ofToUpper(s);
							if (ui.AddButton(s, OFX_IM_BUTTON_BIG))
							{
								doClearSubsList();
							}
						}
					}
					//ui.PopFontStyle();
				}
			}
			/*
			if (ui.isMaximized() && ui.bDebug)
			{
				ui.AddSpacingSeparated();

				static bool bTester = 0;
				ImGui::Checkbox("Tester##1", &bTester);
				if (bTester) {
					if (ui.AddButton("Random", OFX_IM_BUTTON_BIG)) {
						doRandomInput();
					}
				}
			}
			*/
#endif
		}

		if (!ui.isGameMode()) ui.AddSpacingSeparated();

		if (ui.AddButton("Reset All##all"))
		{
			doResetAll();
		}

		ui.EndWindow();
	}
}

//--------------------------------------------------------------
void ofApp::drawImGui()
{
#ifdef USE_SURF_SUBTITLES
	subs.drawGui();
#endif

	//--

	ui.Begin();
	{
		bIsInteractingWindowConversation = 0;

		//--

		//TODO: Fix layer sorting/active.
		// Not working..
		// send conversation window to background in z-order
#if 0
		if (ofGetFrameNum() % 120 == 0)
		{
			const char* windowName = bGui_GptConversation.getName().c_str();
			// The name of the window to send to background
			// ImGuiContext* _context = ImGui::GetCurrentContext();
			ImGuiContext* GImGui = NULL;
			ImGuiContext& _context = *GImGui;

			ImVector<ImGuiWindow*> windows;
			for (ImGuiWindow* window : _context.WindowsFocusOrder)
				if (window->WasActive)
					windows.push_back(window);

			// Iterate over each ImGui window and check its name
			for (int i = 0; i < windows.size(); i++)
			{
				const char* name = windows[i]->Name;
				if (strcmp(name, windowName) == 0)
				{
					// This is the window we want to move to the background
					ImGui::SetWindowFocus(nullptr);
					ImVec2 pos = ImVec2(0, 0); // Set the position to the back of the z-order
					ImGui::SetWindowPos(name, pos);
				}
			}
		}
#endif

		//--

		// TextInput bubble widget
		// Attach a window to a viewport = prevent popping it out
		if (bigTextInput.isVisible()) {
			//auto p = ImVec2{ (float)ofGetWindowPositionX(), (float)ofGetWindowPositionY() };

			{
				//TODO:
				// get moved window listener from ofxWindowApp

				ImGuiCond cond = ImGuiCond_Always;

				const ImGuiViewport* viewport = ImGui::GetMainViewport();

				//int locationWindowContext = -1;
				//const float PAD = 10.0f;
				//ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
				//ImVec2 work_size = viewport->WorkSize;
				//ImVec2 window_pos, window_pos_pivot;
				//window_pos.x = (locationWindowContext & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
				//window_pos.y = (locationWindowContext & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
				//window_pos_pivot.x = (locationWindowContext & 1) ? 1.0f : 0.0f;
				//window_pos_pivot.y = (locationWindowContext & 2) ? 1.0f : 0.0f;
				//ImGui::SetNextWindowPos(window_pos, cond, window_pos_pivot);

				ImGui::SetNextWindowPos(viewport->WorkPos, cond, ImVec2{ 0.f,0.f });
				ImGui::SetNextWindowViewport(viewport->ID);
			}

			//ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
		}
		bigTextInput.draw(ui);

		//--

		if (bGui)
		{
			//--

			// Editor Input

#ifdef USE_EDITOR_INPUT
			editorInput.drawImGui();
#endif

			//--

			//if (ui.isMaximized() && ui.isExtraEnabled())
			//if (ui.isExtraEnabled())
			//if (ui.isDebug())

			//drawImGuiReply(ui);

			//--

#ifdef USE_EDITOR_RESPONSE
			// Editor Response
			editorLastResponse.drawImGui();
#endif
			//--

#ifdef USE_SURF_SUBTITLES
			subs.drawImGui();
#endif

			//--

			// Draw
			// ImGui widgets not windowed

#ifdef USE_PRESETS
			presets.drawImGui(1);
#endif
		}

		//--

		drawImGuiConversation(ui);

		//--

		drawImGuiMain();

		//--

		// Context menu

		//TODO:
		//if (!ui.isGameMode() && ui.isDebug()) ImSpinner::demoSpinners();

		// Open context menu/window
		// when the user right-clicks on the application window

		// // Popup
		// if (ui.isMouseOverAppWindowRightClicked())
		//    {
		//        ImGui::OpenPopup("my_popup"); //momentary
		//    }
		//    bool b = ImGui::IsPopupOpen("my_popup");
		//    if(b) drawWidgetsContextMenu2();

		// // Toggle
		// if (ui.isMouseOverAppWindowRightClicked())
		// {
		//     bGui_WindowContextMenu = !bGui_WindowContextMenu;
		// }
		// bool b = ImGui::IsPopupOpen("my_popup");
		// if (b) drawWidgetsContextMenu2();
		// // drawWidgetsContextMenu();

		// Toggle
		if (ui.isMouseOverAppWindowRightClicked())
		{
			bGui_WindowContextMenu = !bGui_WindowContextMenu;
		}
		if (bGui_WindowContextMenu) drawWidgetsContextMenu2();
	}

	ui.End();
}

/*
//--------------------------------------------------------------
void ofApp::drawImGuiReply(ofxSurfingGui& ui)
{
	// Gpt last reply
	if (ui.BeginWindow(bGui_GptLastReply, ImGuiWindowFlags_None))
	{
		if (ui.isDebug()) ui.AddComboFontsSelector(fontR);

		ui.PushFontStyle(SurfingFontTypes(fontR.get()));

		string s = textLastResponse;

		ImGui::TextWrapped(s.c_str());

		ui.PopFontStyle();

		ui.EndWindow();
	}
}
*/

//--------------------------------------------------------------
void ofApp::doRefreshWindowConversationIfFlagged()
{
	//TODO: this code is more related to reset window behavior. in this case it acts a refresh...
	if (!bRefreshWindowsLayout) return; //TODO: should check if will load settings when first setup
	ofLogNotice("ofApp") << "doResetWindowConversationIfFlagged";

	float padx = spacingX * ofGetWindowWidth() / 2;
	// float padx = spacingX * bigTextInput.getWindowRectangle().getWidth() / 2;
	// float padx = 75; //TODO: x spacing to app borders. hardcoded

	float pady2 = 10; //to the top or bottom window border

	// y spacing between bubble text and window conversation
	// float pady = 25;
	// related to bubble height
	float pady1 = spacingY * bigTextInput.getWindowRectangle().getHeight();

	// a hardcoded same that window header height
	float pady0 = 0;
	pady0 += 2 * ImGui::GetStyle().WindowBorderSize;
	pady0 += ui.getWidgetsHeightUnit();

	float x;
	float y;
	float w;
	float h;

	x = padx;
	w = ofGetWidth() - 2 * padx;

	//TODO: when text input is at the top half
	//ratio to the top border
	if (!bBottomTextInput) // top
	{
		y = bigTextInput.getWindowRectangle().getBottom() + (pady1 + pady0);
		h = ofGetHeight() - y - pady2;
	}
	else // bottom
	{
		y = pady2 + pady0;
		h = ofGetHeight() - (ofGetHeight() - bigTextInput.getWindowRectangle().getTop());
		h -= pady2 + pady1 + pady0;
	}

	h = ofClamp(h, 5 * ui.getWidgetsHeightUnit(), ofGetHeight() - bigTextInput.getWindowRectangle().getHeight());

	ImGui::SetNextWindowSize(ImVec2(w, h), bRefreshWindowsLayout ? ImGuiCond_Always : ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(x, y), bRefreshWindowsLayout ? ImGuiCond_Always : ImGuiCond_FirstUseEver);
	if (bRefreshWindowsLayout) bRefreshWindowsLayout = 0;
}

//--------------------------------------------------------------
void ofApp::drawImGuiConversation(ofxSurfingGui& ui)
{
	if (!bGui_GptConversation) return;

	ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);

	bool b = ui.bDebug;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
	if (!b) window_flags |= ImGuiWindowFlags_NoBackground;
	if (!b) window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (!b) window_flags |= ImGuiWindowFlags_NoResize;
	if (!b && bLock) window_flags |= ImGuiWindowFlags_NoMove;
	//if (!b && bLock) window_flags |= ImGuiWindowFlags_NoMouseInputs;

	//--

	// Reset if required
	doRefreshWindowConversationIfFlagged();

	// Scale the scrollbar size
	float scrollbarSize = ImGui::GetStyle().ScrollbarSize;
	float scrollbarRatio = 1.5f;
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, scrollbarSize * scrollbarRatio);

	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, colorAccent.get());
	ImVec4 ch(colorAccent.get().r / 255.f, colorAccent.get().g / 255.f, colorAccent.get().b / 255.f, 0.5f);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ch);

	//--

	if (ui.BeginWindow(bGui_GptConversation, window_flags))
	{
		if (!ui.bDebug) AddHeaderHeight();

		if (bScaleWindow) {
			float v = 1.f;
			if (scaleWindow.get() != 0.f) v = ofMap(scaleWindow, -1, 1, 0.4f, 4.f, true);
			ImGui::SetWindowFontScale(v);
		}
		else ImGui::SetWindowFontScale(1);

		ui.PushFontByIndex();

		//TODO: should check interaction not focused/visible
		// bIsInteractingWindowConversation = ImGui::IsWindowFocusedImGuiFocusedFlags_RootAndChildWindows);

		//// Context menu
		//if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		//{
		//	ImGui::OpenPopup("my_popup");
		//}
		//drawWidgetsContextMenu();

		//--

		// Scroll control
		{
			float scrollY = ImGui::GetScrollY();

			//TODO:
			if (bFlagGoTop)
			{
				bFlagGoTop = 0;
				ImGui::SetScrollHereY(0.0f);
			}

			// Define the amount of scroll for each arrow key press
			const float scroll_amount = 5 * ImGui::GetTextLineHeightWithSpacing();
			// Check whether up or down arrow key is pressed

			// TODO: centralize all scroll commands
			// Keys ctrl + UP / DOWN
			bool bModControl = ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl));
			if (bModControl)
			{
				if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
				{
					// Decrement the vertical scroll position
					float new_scroll_y = ImGui::GetScrollY() - scroll_amount;
					ImGui::SetScrollY(new_scroll_y);
			}
				else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
				{
					// Increment the vertical scroll position
					float new_scroll_y = ImGui::GetScrollY() + scroll_amount;
					ImGui::SetScrollY(new_scroll_y);
				}
		}
	}

		////TODO:
		//// Send to z-order background
		//bool isWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		//ImGui::SetWindowFocus(nullptr);
		//if(bLock) ImGui::SetWindowFocus(nullptr);

		//--

#if 0
		//if (ui.isDebug()) ui.AddComboFontsSelector(sizeFontConv);
		ui.PushFontStyle(SurfingFontTypes(sizeFontConv.get()));
#endif
		// Colorized by roles

		ImU32 c1 = ImGui::GetColorU32(colorUser.get());
		ImU32 c2 = ImGui::GetColorU32(colorAssistant.get());
		ImU32 c3 = ImGui::GetColorU32(ImVec4(colorUser.get().r, colorUser.get().g, colorUser.get().b,
			colorUser.get().a * 0.7));

		int i = 0;
		for (auto& j : jConversationHistory)
		{
			string role = j["message"]["role"].get<std::string>();
			string content = j["message"]["content"].get<std::string>();
			string type = j["message"]["type"].get<std::string>();

			bool bLast = (jConversationHistory.size() - 1 == i);

			bool bSystem = (type == "system");

			// ImU32 c = (role == "user") ? c1 : c2;
			ImU32 c = IM_COL32(255, 0, 0, 255);
			if (role == "assistant") c = c2;
			else if (role == "user") c = bSystem ? c3 : c1;

			ImGui::PushStyleColor(ImGuiCol_Text, c);

			// Customize last text block
			if (bLast)
			{
				if (bLastBlink) ui.BeginBlinkText(true, 1);
#if 0
				if (bLastBigger) ui.PushFontStyle(SurfingFontTypes(MAX(3, sizeFontConv.get() + 1)));
#endif
			}

			//TODO: process list and style
			bool bList = (type == "list");
			if (bList)
			{
			}

			ImGui::TextWrapped("%s", content.c_str());

			//TODO: process list and style
#if 0
			if (bList)
			{
				bool isHovered = ImGui::IsItemHovered();
				bool isClicked = ImGui::IsItemClicked();

				if (isClicked)
				{
					doGptSendMessage(content);
				}
				if (isHovered)
				{
					// ui.AddLabelLinkURL();
					// ui.AddLinkUrlButton();
				}
			}
#endif
			if (bLast)
			{
				if (bLastBigger) ui.PopFontStyle();
				if (bLastBlink) ui.EndBlinkText();
			}
			ImGui::PopStyleColor();

			i++;
		}
#if 0
		ui.PopFontStyle();
#endif

		//--

#if 1
		if (bFlagGoBottom)
		{
			bFlagGoBottom = 0;
			// Scroll to the bottom of the window
			ImGui::SetScrollHereY(1.0f);
		}
#elif 0
		//TODO:
		// Tween
		if (bFlagGoBottom)
		{
			bFlagGoBottom = 0;

			//if (ImGui::Button("Scroll to bottom"))
			{
				float duration = 1.0f; // Set the duration of the scrolling animation in seconds
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				float start_scroll_y = ImGui::GetScrollY();
				float end_scroll_y = ImGui::GetWindowHeight() + ImGui::GetWindowPos().y - ImGui::GetContentRegionMax().y;

				float t = 0.0f;
				while (t < duration)
				{
					t += ImGui::GetIO().DeltaTime;

					float lerp_value = t / duration;
					float current_scroll_y = ofLerp(start_scroll_y, end_scroll_y, lerp_value);

					ImGui::SetScrollY(current_scroll_y);
					window->DC.CursorPos.y = ImGui::GetContentRegionMax().y;

					// Render the window to update the scrollbar position
					ImGui::Render();
				}

				// Set the scroll position to the end value once the animation is complete
				ImGui::SetScrollY(end_scroll_y);
				window->DC.CursorPos.y = ImGui::GetContentRegionMax().y;
			}
			// ##elif(0)
			//const float scroll_amount = ImGui::GetScrollY() * 0.1;
			//float new_scroll_y = ImGui::GetScrollY() + scroll_amount;
			//ImGui::SetScrollY(new_scroll_y);

			//cout << "ImGui::GetScrollY():" << ImGui::GetScrollY() << endl;


			//float y = ImGui::GetScrollY();
			//float step = 0.1f;
			//if (y < 1.0f) y += step;
			//else if (y >= 1.f) {
			//	bFlagGoBottom = 0;//done
			//	y = 1.f;
			//	cout << "y:" << y << endl;
			//}
			////ImGui::SetScrollY(y);
			//ImGui::SetScrollHereY(y);
		}

		//if (bFlagGoBottom)
		//{
		//	// Define a boolean flag to trigger the scroll animation
		//	bool shouldScroll = 1;

		//	// Define the target scroll position
		//	float targetScrollY = 1;

		//	// Define the duration of the scroll animation (in seconds)
		//	float duration = 0.5f;

		//	// Define the start time of the scroll animation
		//	float startTime = 0.0f;

		//	// Update the scroll position
		//	if (shouldScroll)
		//	{
		//		// Set the start time of the animation
		//		startTime = ImGui::GetTime();

		//		// Clear the flag to prevent triggering the animation again
		//		shouldScroll = false;
		//	}

		//	// Calculate the elapsed time since the start of the animation
		//	float elapsedTime = ImGui::GetTime() - startTime;

		//	// Calculate the current scroll position based on the elapsed time and the target position
		//	float currentScrollY = ofLerp(0.0f, targetScrollY, elapsedTime / duration);

		//	// Set the current scroll position
		//	ImGui::SetScrollY(currentScrollY);

		//	if (currentScrollY == targetScrollY) bFlagGoBottom = 0;
		//}
#endif

		ui.PopFontByIndex();

		ui.EndWindow();
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleVar();
}

//--

#ifdef USE_SURF_SUBTITLES
//--------------------------------------------------------------
void ofApp::doPopulateText(string s)
{
	// workflow
	doClearSubsList();

	// auto generate a random text
	if (s == "")
	{
		s = ofxSurfingHelpers::getTextRandom();

		// workaround log fix
		s = " " + s;
	}

	////TODO:
	////trick
	//ui.ClearLogDefaultTags();
	//ofColor c = ofColor(subs.getColorText(), 255);
	//ui.AddLogTag(c);

#ifdef USE_SURF_SUBTITLES
	//ofLogNotice() << s;
	subs.doSetTextSlideStart(s);
	ui.AddToLog(s);
#endif

	// Spacing
	for (size_t i = 0; i < 10; i++)
	{
		ofLogNotice("ofApp") << "|";
	}
		}

// Function to process a full file and split into blocks/slides.
//--------------------------------------------------------------
void ofApp::doPopulateTextBlocks() {
	string path = "files/txt/text2.txt";
	subs.setupTextBlocks(path);
}
//--------------------------------------------------------------
void ofApp::doClearSubsList() {
	subs.doClearList();
}
#endif

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	ofLogNotice("ofApp") << "keyPressed:" << char(key);

	//TODO:
	//if (ui.isMouseOverInputText())
	if (bigTextInput.isMouseOverInputText())
	{
		doAttendCallbackKeys();
	}

	//--

	auto doToggleUI = [this]()
	{
		bGui = !bGui;

		// workflow
		if (!bGui && bigTextInput.bGui_Config) bigTextInput.bGui_Config = 0;
	};

	//if (key == 'g' || key == 'G') {
	if (key == OF_KEY_F1)
	{
		doToggleUI();

		/*
		bGui = !bGui;

		// workflow
		if (!bGui && bigTextInput.bGui_Config) bigTextInput.bGui_Config = 0;
		*/
	}

	//----

	if (ui.isMouseOverInputText()) return; // skip when editing
	if (chatGpt.isWaiting()) return;

#ifdef USE_OFX_ELEVEN_LABS
	// For the testing Helpers
	tts.keyPressed(key);
#endif

	if (0) return;

	else if (key == 'g' || key == 'G') { doToggleUI(); }

	else if (key == OF_KEY_PAGE_UP) { bFlagGoTop = 1; }
	else if (key == OF_KEY_PAGE_DOWN) { bFlagGoBottom = 1; }
	//else if (key == OF_KEY_UP) { bFlagGoTopStp = 1; }
	//else if (key == OF_KEY_DOWN) { bFlagGoBottomStp = 1; }

	// Focus in text input widget
	else if (key == OF_KEY_TAB) { bigTextInput.setFocus(); }

	// Clear
	else if (key == OF_KEY_BACKSPACE) { doClear(); }

	//else if (key == OF_KEY_RETURN) { bigTextInput.doEnterKey(); }

	// Resend last
	//else if (key == OF_KEY_RETURN) { doGptResend(); }

	//// Regenerate
	//else if (key == OF_KEY_RETURN) { doGptRegenerate(); }
	//else if (key == ' ') { doGptRegenerate(); }

	bool bModControl = ofGetKeyPressed(OF_KEY_CONTROL);
	if (!bModControl)
	{
		//TODO:
		// Browse history
		if (key == OF_KEY_DOWN)
		{
			if (textHistory.size() == 0) return;
			int i_last = textHistory.size() - 1;
			i_hist--;
			i_hist = ofClamp(i_hist, 0, textHistory.size() - 1);
			string s = textHistory[i_last - i_hist];
			bigTextInput.setText(s);
			textInput.setWithoutEventNotifications(s);
			//bigTextInput.setFocus();
		}
		else if (key == OF_KEY_UP)
		{
			if (textHistory.size() == 0) return;
			int i_last = textHistory.size() - 1;
			i_hist++;
			i_hist = ofClamp(i_hist, 0, textHistory.size() - 1);
			string s = textHistory[i_last - i_hist];
			bigTextInput.setText(s);
			textInput.setWithoutEventNotifications(s);
			//bigTextInput.setFocus();
		}
	}

	//--

	//TODO: tester
	if (key == '1')
	{
		strBandname = "Fugazi";
		doGptSendMessage(strBandname);
		return;
	}
	if (key == '2')
	{
		strBandname = "The Smiths";
		doGptSendMessage(strBandname);
		return;
	}
	if (key == '3')
	{
		strBandname = "Radio Futura";
		doGptSendMessage(strBandname);
		return;
	}
	if (key == '4')
	{
		strBandname = "John Frusciante";
		doGptSendMessage(strBandname);
		return;
	}
	if (key == '5')
	{
		strBandname = "Portishead";
		doGptSendMessage(strBandname);
		return;
	}
	if (key == '6')
	{
		strBandname = "Jane's Addiction";
		doGptSendMessage(strBandname);
		return;
	}
	if (key == '7')
	{
		strBandname = "Joy Division";
		doGptSendMessage(strBandname);
		return;
	}
	if (key == '8')
	{
		strBandname = "Primus";
		doGptSendMessage(strBandname);
		return;
	}
	if (key == '9')
	{
		strBandname = "Kraftwerk";
		doGptSendMessage(strBandname);
		return;
	}

	//----

	if (ui.isGameMode()) return; // skip

	if (key == 'd')
	{
		ui.bDebug = !ui.bDebug;
		bigTextInput.bDebug = ui.bDebug;
	}

	else if (key == 'l') { bLock = !bLock; }

	//--

#ifdef USE_SURF_SUBTITLES
	else if (key == 'e') { subs.setToggleEdit(); }
	//else if (key == 'l') { subs.setToggleLive(); }
	//else if (key == 'g') { subs.setToggleVisibleGui(); }
	//else if (key == ' ') { doPopulateText(); }

	//else if (key == ' ') { subs.setTogglePlay(); }
	//else if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }
	//else if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	//else if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	//else if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };
#endif

	//--

	//     //TODO:
	// #ifdef USE_EDITOR_INPUT
	//     //switch (key)
	//     //{
	//     //case '1': {
	//     //	string path = ofToDataPath("text1.txt", true);
	//     //	editorInput.loadText(path);
	//     //	break;
	//     //}
	//     //case '2': {
	//     //	string path = ofToDataPath("text2.txt", true);
	//     //	editorInput.loadText(path);
	//     //	break;
	//     //}
	//     //case '3': {
	//     //	string str = "Garc�a Castell�n pone la X de Kitchen a Fern�ndez D�az\n y tapona la investigaci�n a Rajoy, \nla c�pula del PP y \nel CNI El juez \ndetermina que la decisi�n \nde espiar a B�rcenas con \nfondos reservados para evitar problemas judiciales \nal presidente y a Cospedal no \ntrascendi� del Ministerio del Interior.\nEl cierre de la instrucci�n llega \ncuando Anticorrupci�n apunta al CNI en \nel episodio del 'falso cura\n' e investiga una segunda Kitchen \nen la c�rcel";
	//     //	editorInput.setText(str);
	//     //	break;
	//     //}
	//     //}
	// #endif
}

#ifdef USE_EDITOR_INPUT
//--------------------------------------------------------------
void ofApp::drawWidgetsEditor()
{
	//--

	//if (editorInput.bExtra || !editorInput.bMenus) ui.AddSpacingSeparated();

	//float w = 200;
	float w = ui.getWidgetsWidth(4);
	float h = 2 * ui.getWidgetsHeightUnit();
	ImVec2 sz{ w, h };

	if (ui.AddButton("SEND", sz))
	{
		string s = editorInput.getText();
		ui.AddToLog(s, OF_LOG_NOTICE);
		doGptSendMessage(s, bModeConversation);

		// workflow
		//editorInput.clearText();
	};
	ui.SameLine();
	if (ui.AddButton("CLEAR", sz))
	{
		editorInput.clearText();
	};

	ui.Add(bModeConversation);

	//editorInput.drawImGuiWidgetsFonts();

	ui.AddSpacingSeparated();

	//--

	// Catch from previous widget
	//bOver = ImGui::IsItemHovered();
	ImGuiIO& io = ImGui::GetIO();

	bool bMouseLeft = io.MouseClicked[0];
	bool bMouseRight = io.MouseClicked[1];
	float mouseWheel = io.MouseWheel;

	//bool bKeyReturn = ImGui::IsKeyboardKey(ImGuiKey_Enter);
	bool bKeyReturn = ImGui::IsKeyPressed(ImGuiKey_Enter);
	bool bCtrl = io.KeyCtrl;
	bool bShift = io.KeyShift;

	// Enter
	if (bCtrl && bKeyReturn)
	{
		string s = editorInput.getText();
		ui.AddToLog(s, OF_LOG_NOTICE);
		doGptSendMessage(s, bModeConversation);
		editorInput.clearText();
	}
	}
#endif

//--------------------------------------------------------------
void ofApp::doSendSilentMessageToConversation(string message)
{
	ofLogNotice("ofApp") << message;

	ofJson j;
	j["message"]["role"] = "user";
	j["message"]["content"] = message;
	j["message"]["type"] = "system";

	ofLogNotice("ofApp") << message;

	jConversationHistory.push_back(j);
}

//--------------------------------------------------------------
void ofApp::doGptSendMessage(string message)
{
	ofLogNotice("ofApp") << message;

	// Clear

	ofxChatGPT::ErrorCode errorCode;
	bGptError = false;

	ofJson j;
	j["message"]["role"] = "user";
	j["message"]["content"] = message;
	j["message"]["type"] = "question"; //not used

	jConversationHistory.push_back(j); //set to chat conversation window

	ofLogVerbose("ofApp") << "User: " << message;

	// jQuestion = jMsg;
	jResponse = ofJson();

	bWaitingGpt = 1;

	// clear
	textLastResponse = "";

	if (!bModeConversation)
	{
#ifdef USE_EDITOR_RESPONSE
		editorLastResponse.clearText(); // workflow
#endif
	}

	ui.AddToLog("doGptSendMessage()", OF_LOG_WARNING);
	ui.AddToLog(message);

	//--

	// Submit
	//message = "\n" + message;
	// message = message;
	if (bModeConversation) chatGpt.chatWithHistoryAsync(message);
	else chatGpt.chatAsync(message);

	//textHistory.push_back(message);

	sounds.play(1);

	// workflow

	// scroll conversation
	bFlagGoBottom = 1;

	// focus in text input
	bigTextInput.setFocus();

	i_hist = 0;
}

//--------------------------------------------------------------
void ofApp::doGptResend()
{
	if (textHistory.size() == 0) return;

	ui.AddToLog("doGptResend()", OF_LOG_WARNING);

	string s = textHistory.back();
	textInput.set(s);
	//doAttendCallbackTextInput();
}

//--------------------------------------------------------------
void ofApp::doGptRegenerate()
{
	ui.AddToLog("doGptRegenerate()", OF_LOG_WARNING);
	sounds.play(5);

	chatGpt.regenerateAsync();
}

//--------------------------------------------------------------
void ofApp::doGptDoContinue()
{
	ofLogNotice("ofApp");
	ui.AddToLog("doGptDoContinue()", OF_LOG_WARNING);
	doGptSendMessage("continue");
}

//--------------------------------------------------------------
void ofApp::doGptDoAJoke()
{
	ofLogNotice("ofApp");
	ui.AddToLog("doGptDoAJoke()", OF_LOG_WARNING);

	//TODO: to allow resend las original response, 
	// not the joke/summarization itself

	//if(!bLastWasATrick) textLastTrick = textLastResponse;

	int max_words = 10;

	string s = "Do a joke based on the following quote:\n";
	s += "\"" + textLastResponse + "\"";
	s += "\nThe joke must be short, less than " + ofToString(max_words) + " words.";
	s += "\nStart your answer by warning that you are going to make a joke.";

	s += "\nBefore return the final text, remove break lines: put all sentenced in one single text line.";
	//s += "\nDo a double break line starting the whole reply. But put all the others on a single line. Do not add extra spaces between lines.";

	// for ElevenLabs variations
	//s += "\nPut your sentence between \" \" quotes. And then, add at the end: ";
	//s += "\n he said, smiling with laughter.";

	//bool bSilent = 1;
	//if (!bSilent) doGptSendMessage(s);
	//else
	//{
	//	if (bModeConversation) chatGpt.chatWithHistoryAsync(s);
	//	else chatGpt.chatAsync(s);
	//}

	if (bModeConversation) chatGpt.chatWithHistoryAsync(s);
	else chatGpt.chatAsync(s);


	// workflow
	sounds.play(1);
	// scroll conversation
	bFlagGoBottom = 1;
	// focus in text input
	bigTextInput.setFocus();

	doSendSilentMessageToConversation("JOKE:");
}

//--------------------------------------------------------------
void ofApp::doGptDoASummarization()
{
	ofLogNotice("ofApp");
	ui.AddToLog("doGptDoASummarization()", OF_LOG_WARNING);

	int max_words = 10;

	string s = "Do a short summarization of the following quote:\n";
	s += "\"" + textLastResponse + "\"";
	s += "\nThe summarization must be short, less than " + ofToString(max_words) + "  words.";
	s += "\nStart your answer by saying: \"let me summarize in one sentence.\"";
	s += "\nDo a break line starting the reply.";

	// set response size
	s += sLimit();

	// send
	bool bSilent = 1;
	if (!bSilent) doGptSendMessage(s);
	else
	{
		if (bModeConversation) chatGpt.chatWithHistoryAsync(s);
		else chatGpt.chatAsync(s);
	}

	// workflow
	sounds.play(1);
	// scroll conversation
	bFlagGoBottom = 1;
	// focus in text input
	bigTextInput.setFocus();
	// text
	doSendSilentMessageToConversation("SUMMARIZATION:");
}

//--------------------------------------------------------------
void ofApp::doGptGetMessage()
{
	ui.AddToLog("doGptGetMessage()", OF_LOG_WARNING);

	// Get
	string strGptResponse;
	ofxChatGPT::ErrorCode errorCode;
	tie(strGptResponse, errorCode) = chatGpt.getMessage();

	indexErrorCode = getErrorCodeByCode(errorCode);
	if (indexErrorCode > 0 && indexErrorCode < 9) bGptError = true;
	else bGptError = false;

	bWaitingGpt = 0;

	if (errorCode == ofxChatGPT::Success) // Success
	{
		// Get response

		ofLogNotice("ofApp") << "ofxChatGPT Success.";
		bGptError = false;
		gptErrorMessage = "State: Success";
		indexErrorCode = 0;

		ofJson j;
		j["message"]["role"] = "assistant";
		j["message"]["content"] = strGptResponse;

		if (indexPrompt == 0) j["message"]["type"] = "text";
		else j["message"]["type"] = "list";

		jConversationHistory.push_back(j);

		ofLogNotice("ofApp") << "Assistant: \n" << strGptResponse;

		jResponse = j;

		//--

		//TODO:
		// Process response
		for (auto& content : j["content"])
		{
			strGptResponse += content.get<std::string>() + "\n";
		}
		//ofLogNotice("ofxSurfingTextSubtitle") << "strGptResponse:" << strGptResponse;

		//--

		textLastResponse = ofxSurfingHelpers::removeNumbersStartingLines(strGptResponse);
		ofLogNotice("ofApp") << textLastResponse;
		ui.AddToLog(textLastResponse, OF_LOG_NOTICE);

		//--

#ifdef USE_SURF_SUBTITLES
		// Build slides
		if (!bModeConversation)
		{
			if (bModeOneSlide) subs.doBuildDataTextOneSlideOnly(textLastResponse);
			else subs.doBuildDataText(textLastResponse);
		}
#endif

		/*
#ifdef USE_SURF_SUBTITLES
		//TODO:
		//there's no new line \n marks. so we assume the blocks will be numbered 1., 2., 3. etc
		size_t sz = ofxSurfingHelpers::countNewlines(textLastResponse);
		bool b = true;
		//b = sz == 0;
		if (b) {
			subs.doBuildDataTextBlocks(textLastResponse, true);
		}
		else {//we found \n tags. so we assume blocks ends with \n.
			subs.doBuildDataTextBlocks(textLastResponse);
		}
#endif
		*/

		//--

#ifdef USE_EDITOR_RESPONSE
		// Here textLastResponse is already cached 
		editorLastResponse.clearText(); // workflow
		editorLastResponse.addText(textLastResponse + "\n");
#endif
		sounds.play(3);

#ifdef USE_OFX_ELEVEN_LABS
		tts.doSend(textLastResponse);
#endif
		}
	else // Error
	{
		//tuple<string, ofxChatGPT::ErrorCode> m = ofxChatGPT::getMessage();

		indexErrorCode = getErrorCodeByCode(errorCode);
		bGptError = true;
		gptErrorMessage = "Error: " + ofxChatGPT::getErrorMessage(errorCode);
		ofLogError("ofApp") << "ofxChatGPT has an error: " << gptErrorMessage;
		ui.AddToLog(gptErrorMessage, OF_LOG_ERROR);

		//--

		// workaround to try to fix the error on the fly
		// bc error could like new IP renew for our endpoint server.
		bool b = doGptResetEndpointIP();
		if (b) ui.AddToLog("doGptResetEndpointIP() Success", OF_LOG_WARNING);
		else ui.AddToLog("doGptResetEndpointIP() Error", OF_LOG_ERROR);

		sounds.play(3);
	}

	// workflow

	// scroll conversation
	bFlagGoBottom = 1;

	// focus in text input
	bigTextInput.setFocus();
	}

//--------------------------------------------------------------
void ofApp::doRandomInput()
{
	ui.AddToLog("doRandomInput()", OF_LOG_WARNING);

	// workflow
#ifdef USE_SURF_SUBTITLES
	doClearSubsList();
#endif

	size_t sz = 9;
	float r = ofRandom(sz);
	if (r < 1) { strBandname = "Jane's Addiction"; }
	else if (r < 2) { strBandname = "Fugazi"; }
	else if (r < 3) { strBandname = "Joy Division"; }
	else if (r < 4) { strBandname = "The Smiths"; }
	else if (r < 5) { strBandname = "Radio Futura"; }
	else if (r < 6) { strBandname = "John Frusciante"; }
	else if (r < 7) { strBandname = "Primus"; }
	else if (r < 8) { strBandname = "Kraftwerk"; }
	else if (r < 9) { strBandname = "Portishead"; }
	string s = "";
	s = strBandname;

	//textInput = s;

	ui.AddToLog(s, OF_LOG_NOTICE);

	bigTextInput.setText(s);
	doGptSendMessage(s);

#ifdef USE_EDITOR_INPUT
	editorInput.setText(s);
	ui.AddToLog("editorInput.setText()");
	ui.AddToLog(s, OF_LOG_NOTICE);
	doGptSendMessage(editorInput.getText(), bModeConversation);
#endif
};

//--------------------------------------------------------------
void ofApp::doSwapGptPrompt()
{
	if (indexPrompt == 0) indexPrompt = 1;
	else if (indexPrompt == 1) indexPrompt = 2;
	else if (indexPrompt == 2) indexPrompt = 3;
	else if (indexPrompt == 3) indexPrompt = 0;
}

//--------------------------------------------------------------
void ofApp::exit()
{
	ofLogVerbose() << "exit()";

	ofRemoveListener(ofEvents().windowResized, this, &ofApp::windowResized);
	ofRemoveListener(params.parameterChangedE(), this, &ofApp::Changed_Params);

	//ofxSurfingHelpers::save(params);

	ofJson configJson;
	configJson["apiKey"] = apiKey;
	configJson["model"] = model;
	ofSavePrettyJson(pathGptSettings, configJson);

	w.save();
}

//--------------------------------------------------------------
int ofApp::getErrorCodeByCode(ofxChatGPT::ErrorCode errorCode)
{
	int i = -1;

	switch (errorCode)
	{
	case ofxChatGPT::Success: i = 0;
		break;
	case ofxChatGPT::InvalidAPIKey: i = 1;
		break;
	case ofxChatGPT::NetworkError: i = 2;
		break;
	case ofxChatGPT::ServerError: i = 3;
		break;
	case ofxChatGPT::RateLimitExceeded: i = 4;
		break;
	case ofxChatGPT::TokenLimitExceeded: i = 5;
		break;
	case ofxChatGPT::InvalidModel: i = 6;
		break;
	case ofxChatGPT::BadRequest: i = 7;
		break;
	case ofxChatGPT::Timeout: i = 8;
		break;
	case ofxChatGPT::UnknownError: i = 9;
		break;
	default: i = 9;
		break;
	}

	return i;
}

//--------------------------------------------------------------
void ofApp::doAttendCallbackClear()
{
	ofLogNotice("ofApp") << "doAttendCallbackClear";

	//TODO:
	// workflow
	doClear();
	// doClearAll();
	bigTextInput.setFocus();
}

//--------------------------------------------------------------
void ofApp::doAttendCallbackTextInput()
{
	ofLogNotice("ofApp") << "doAttendCallbackTextInput";

	// workflow
	//clear
#ifdef USE_SURF_SUBTITLES
	doClearSubsList();
#endif

	if (!bModeConversation)
	{
#ifdef USE_EDITOR_RESPONSE
		editorLastResponse.clearText();
#endif
	}

	//// will be called when submitted text changed!
	//text = bigTextInput.getText();
	//ofLogNotice("ofApp") << text;
	//ofSetWindowTitle(text);

	//ui.AddToLog("TextInput: " + textInput.get(), OF_LOG_NOTICE);

	v = 1;

	string s = textInput.get();
	ui.AddToLog("Prompt: " + s, OF_LOG_WARNING);

	textHistory.push_back(s);

	doGptSendMessage(s);
}

//--------------------------------------------------------------
void ofApp::doGptRestart()
{
	ofLogNotice("ofApp") << "doGptRestart";
	sounds.play(5);

	setupGpt();

	doClear(1);

	setupGptPrompts();
	doGptSetPrompt(indexPrompt);
}

//--------------------------------------------------------------
bool ofApp::doGptResetEndpointIP()
{
	ofLogNotice("ofApp");
	sounds.play(5);

	CURL* curl;
	CURLcode res;
	struct curl_slist* headers = NULL;

	const std::string api_key = apiKey;

	// Set up the headers
	std::string authorization_header = "Authorization: Bearer " + api_key;
	headers = curl_slist_append(headers, authorization_header.c_str());

	// Set up the URL and payload
	std::string url = "https://api.pawan.krd/resetip";
	std::string payload = "";

	curl = curl_easy_init();
	if (curl)
	{
		// Set up the request
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

		// Disable SSL certificate verification
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

		// Send the request
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

			string s = "doGptResetEndpointIP() curl_easy_perform() failed: " + ofToString(curl_easy_strerror(res));
			ui.AddToLog(s, OF_LOG_ERROR);

			// Clean up
			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);

			return false;
		}

		// Clean up
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		return true;
	}

	return false;
}

//--------------------------------------------------------------
void ofApp::doResetAll(bool bSilent)
{
	ofLogNotice("ofApp");
	if (!bSilent) sounds.play(5);

	doClear();

	bGui = 1;
	bGui_GptConversation = 1;
	bigTextInput.bGui_Config = 0;
#ifdef USE_EDITOR_RESPONSE
	editorLastResponse.bGui = 0;
#endif

	bModeConversation = 1;
	sizeFontConv = 2;
	bLastBlink = 1;
	bLastBigger = 0;
	doRefreshWindowsLayout();

	//colorBg = ofColor(39);
	//colorAccent = ofColor(85, 0, 185);
	//colorUser = ofColor(255, 255);
	//colorAssistant = ofColor(200, 255);

	bigTextInput.doResetAll();

	setupGptPrompts();
	doGptSetPrompt(0);
}

//--------------------------------------------------------------
void ofApp::doClearAll()
{
	ofLogNotice("ofApp") << "doClearAll";

	//TODO:
	doGptRestart();

	// //TODO: force
	// chatGpt.stopThread();
	// chatGpt.setup(model, apiKey);
}

//--------------------------------------------------------------
void ofApp::doClear(bool bSilent)
{
	ofLogNotice("ofApp") << "doClear";
	if (!bSilent) sounds.play(4);

	ui.AddToLog("doClear", OF_LOG_WARNING);

	// //TODO:
	// chatGpt.clear();
	// chatGpt.stopThread();
	// // chatGpt.();

	// //TODO: force
	// chatGpt.stopThread();
	// chatGpt.setup(model, apiKey);

	textHistory.clear();

	bWaitingGpt = 0;
	bGptError = 0;
	gptErrorMessage = "";

	jConversationHistory.clear();

#ifdef USE_SURF_SUBTITLES
	doClearSubsList();
#endif

#ifdef USE_EDITOR_RESPONSE
	if (!bModeConversation)
	{
		editorLastResponse.clearText(); // workflow
	}
#endif

	textLastResponse = "";
}

//--------------------------------------------------------------
void ofApp::doAttendCallbackKeys()
{
	ofLogNotice("ofApp") << "doAttendCallbackKeys";

	static float t = 0;
	if (ofGetElapsedTimef() - t > 100) return;
	t = ofGetElapsedTimef();

	sounds.playKeys();
}

//--------------------------------------------------------------
void ofApp::drawWidgetsToTextInput()
{
	ui.AddSpacingY(3);
	ui.Add(bGui, OFX_IM_TOGGLE_ROUNDED_MINI_XS);

	/*
	// we can't add more widgets here. only if we use SameLine
	ImGui::SameLine();
	ImGui::PushItemWidth(0);
	ImGui::Button("My Button");
	ImGui::PopItemWidth();
	*/
}

//--------------------------------------------------------------
void ofApp::drawWidgetsContextMenu2()
{
	// Transparent background

	// Bg
	// ImVec4 bgCol = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
	// ImGui::PushStyleColor(ImGuiCol_PopupBg, bgCol);

	if (1) ImGui::SetNextWindowBgAlpha(0.75f);

	// if (ImGui::BeginPopup("my_popup"))

	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_AlwaysAutoResize |
		// ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav;

	ImGuiCond cond = ImGuiCond_Always;
	// ImGuiCond cond = ImGuiCond_Appearing;

	if (locationWindowContext >= 0)
	{
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (locationWindowContext & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		window_pos.y = (locationWindowContext & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		window_pos_pivot.x = (locationWindowContext & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (locationWindowContext & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(window_pos, cond, window_pos_pivot);
		ImGui::SetNextWindowViewport(viewport->ID);

		window_flags |= ImGuiWindowFlags_NoMove;
	}
	else if (locationWindowContext == -2)
	{
		// Center window
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		window_flags |= ImGuiWindowFlags_NoMove;
	}

	//TODO:
	float w = 600;
	float h = 0;
	ImGui::SetNextWindowSizeConstraints(ImVec2(w, h), ImVec2(FLT_MAX, FLT_MAX));

	if (ui.BeginWindow(bGui_WindowContextMenu, window_flags))
	{
		//TODO:
		// workaround. breaks text input focus
		// Fix focus issues when conversation window 
		// is on top of the z-order blocking other windows.
		if (1)
		{
			if (ofGetFrameNum() % 120 == 0 && !bigTextInput.isMouseOverInputText() && !bIsOver_GuiMain)
			{
				ui.setWindowFocused(bGui_WindowContextMenu);
			}
		}

		//--

		drawWidgetsContextWidgets();

		ui.EndWindow();
	}

	// ImGui::PopStyleColor(); // Bg
}

// //--------------------------------------------------------------
// void ofApp::drawWidgetsContextMenu1()
// {
//     //ui.AddButton("A");
//
//     //ui.PushFontStyle(SurfingFontTypes(1));
//
//     if (ImGui::BeginPopup("my_popup"))
//     {
//         //TODO:
//         float x = 0;
//         float y = 0;
//         float w = 200;
//         float h = 0;
//
//         //ImGui::SetNextWindowSizeConstraints(ImVec2(140, 0), ImVec2(w, 0));
//         ImGui::SetNextWindowSize(ImVec2(w, h));
//         // ImGui::SetNextWindowPos(ImVec2(x, y));
//
//         ui.Add(bGui, OFX_IM_TOGGLE_ROUNDED);
//
//         //ui.Add(ui.bGui_GameMode, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
//         ui.DrawWidgetsFonts(sizeFontConv, 0);
//         if (ui.AddButton("Restart##3", bGptError ? OFX_IM_BUTTON_BORDER_BLINK : OFX_IM_BUTTON))
//         {
//             doGptRestart();
//         }
//         if (ui.AddButton("Clear##3", OFX_IM_BUTTON))
//         {
//             doClear();
//         }
//         ui.AddCombo(indexPrompt, promptsNames);
//         ui.AddTooltip(strPrompt);
//
//         if (indexPrompt != 0)
//         {
//             ui.AddCombo(indexTags, tagsNames);
//             ui.Add(amountResultsPrompt, OFX_IM_STEPPER);
//         }
//
// #ifdef USE_OFX_ELEVEN_LABS
//         ui.Add(tts.bEnable, OFX_IM_TOGGLE);
//         if (tts.bEnable) ui.Add(tts.bModeUseAltServer, OFX_IM_TOGGLE_ROUNDED_MINI);
// #endif
//         ui.AddSpacing();
//
//         if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
//
//         ImGui::EndPopup();
//     }
//
//     //ui.PopFontStyle();
// }

//--

#ifdef USE_WHISPER

//--------------------------------------------------------------
void ofApp::doUpdatedWhisper()
{
	string s = whisper.getTextLast();
	ofLogNotice() << "doUpdatedWhisper(): " << s;
	doPopulateText(s);
}

//--------------------------------------------------------------
void ofApp::drawImGuiWidgetsWhisper()
{
	ui.Add(ui.bMinimize, OFX_IM_TOGGLE_BUTTON_ROUNDED);
	ui.Add(ui.bLog, OFX_IM_TOGGLE_BUTTON_ROUNDED);
	if (ui.bLog || whisper.bDebug) {
		ui.AddSpacing();
		if (ui.Add(whisper.vClear, OFX_IM_BUTTON)) {
			ui.ClearLog();
		};
	}
	ui.AddSpacingBigSeparated();

	ui.AddLabelHuge("ofxWhisper");
	ui.AddSpacing();
	ui.Add(whisper.bEnable, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
	ui.AddSpacing();

	if (ui.isMaximized())
	{
		ui.AddSpacing();
		ui.Add(whisper.bTimeStamps, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
		ui.Add(whisper.bSpanish, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
		s = "Uses another model\n";
		s += "Requires app restart!";
		ui.AddTooltip(s);
		ui.Add(whisper.bHighQuality, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
		s = "Uses a bigger model\n";
		s += "Requires app restart!";
		ui.AddTooltip(s);
		ui.Add(whisper.step_ms);
		s = "Default is 500ms\n";
		s += "Requires app restart!";
		ui.AddTooltip(s);
		ui.Add(whisper.length_ms);
		s = "Default is 5000ms\n";
		s += "Requires app restart!";
		ui.AddTooltip(s);
		ui.AddSpacing();
		ui.Add(whisper.bDebug, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
		if (whisper.bDebug) {
		}
		ui.AddSpacing();
		//ui.AddLabel(whisper.getTextLast());
}

	ui.AddSpacingBigSeparated();
}

#endif

//--------------------------------------------------------------
void ofApp::windowResized(ofResizeEventArgs& resize)
{
	static float _w = -1;
	static float _h = -1;
	bool b = 0;
	if (_w != resize.width)
	{
		_w = resize.width;
		b = 1;
	}
	if (_h != resize.height)
	{
		_h = resize.height;
		b = 1;
	}
	if (!b) return; // skip if not changed

	ofLogVerbose("ofApp::windowResized") << resize.width << "," << resize.height;
	doRefreshWindowsLayout();
}

//--------------------------------------------------------------
void ofApp::doRefreshWindowsLayout()
{
	ofLogNotice("ofApp") << "doRefreshWindowConversation";
	bRefreshWindowsLayout = 1;
}

//--------------------------------------------------------------
void ofApp::doResetWindowsLayout()
{
	ofLogNotice("ofApp") << "doResetWindowConversation";

	bigTextInput.bGui = 1;
	bGui_GptConversation = 1;


	bigTextInput.windowY = 0.02;
	bigTextInput.windowPadX = 0.4;
	spacingX = 0.2;
	spacingY = 0.2;
}

//--------------------------------------------------------------
void ofApp::drawWidgetsContextWidgets()
{
	ui.AddSpacing();

	// ui.AddLabelHugeXXL("surfChat");

	static bool p_open = 1;
	static bool big_font = 1;
	if (big_font) ui.PushFontStyle(SurfingFontTypes(1));
	{
		ui.AddSeparatorText("ROLE");
		ofxImGuiSurfing::AddMatrixClickerLabelsStrings(indexPrompt, promptsNames, true, promptsNames.size());

		if (indexPrompt != 0)
		{
			ui.AddSeparatorText("AREA");
			ofxImGuiSurfing::AddMatrixClickerLabelsStrings(indexTags, tagsNames, true, tagsNames.size() / 2);
		}
		// else
		{
			ui.PushFontStyle(0);

			ui.AddSeparatorText("SIZE");
			ofxImGuiSurfing::AddMatrixClickerLabelsStrings(indexSize, sizeNames, true, sizeNames.size(),
				true, ui.getWidgetsHeight());
			ui.PopFontStyle();
		}

		ui.AddSeparatorText("EXTRA");
		if (ui.AddButton("Continue", OFX_IM_BUTTON, 3))
		{
			doGptDoContinue();
		}
		ui.SameLine();
		if (ui.AddButton("Joke", OFX_IM_BUTTON, 3))
		{
			doGptDoAJoke();
		}
		ui.SameLine();
		if (ui.AddButton("Summarize", OFX_IM_BUTTON, 3))
		{
			doGptDoASummarization();
		}
	}
	if (big_font) ui.PopFontStyle();

	ui.AddSpacingBig();

	//static ofParameter<bool> bMore{"MORE", 0};
	//ui.Add(bMore, OFX_IM_TOGGLE_ROUNDED);
	//if (bMore)

	static bool open = false;
	bool bIndented = true;
	ImGuiTreeNodeFlags flagsTree = ImGuiTreeNodeFlags_None;

	if (ui.BeginTree("##more", open, bIndented, flagsTree))
	{
		//ui.Indent();
		//ui.AddSpacingBig();
		ui.AddSpacingBigSeparated();

		// 1
		ui.BeginColumns(3, "##more");
#if 0
		ui.DrawWidgetsFonts(sizeFontConv, 0);
		ui.AddTooltip("Font Size");
#endif
		//ui.AddThemeToggle();

		static auto drawStyledText = [this]()
		{
			ui.PushFontByIndex();
			{
				// Here we will draw text with the customized font!
				string t = (ui.getFontIndexName());
				ui.AddTooltip(t);
			}
			ui.PopFontByIndex();
		};


		ui.AddLabelBig("FONT");
		ui.AddSpacing();
		string s;
		//s = "FONT " + ofToString(ui.getFontIndex()) + "/" + ofToString(ui.getAmountFonts() - 1);
		//ui.AddLabel(s);
		//ui.AddSpacing();
		// We browse and select the custom index font:
		ui.DrawWidgetsFonts();
		drawStyledText();

		ui.Add(sizeFont, OFX_IM_STEPPER);
		//// Move this to drawing part in your code: 
		//{
		//	ui.AddSpacing();
		//	ui.PushFontByIndex();
		//	{
		//		// Here we will draw text with the customized font!
		//		string t = (ui.getFontIndexName());
		//		ImGui::TextWrapped("%s", t.c_str());
		//	}
		//	ui.PopFontByIndex();
		//}

		ui.Add(bScaleWindow, OFX_IM_TOGGLE_SMALL);
		if (bScaleWindow) {
			if (ImGui::Button("R")) {
				scaleWindow = 0;
			}
			ui.SameLine();
			ui.Add(scaleWindow, OFX_IM_STEPPER);
			ui.Add(scaleWindow, OFX_IM_HSLIDER_MINI_NO_LABELS);
		}
		// 2
		ui.NextColumn();
		ui.AddLabelBig("PANELS");
		ui.AddSpacing();
		ui.Add(ui.bGui_GameMode, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
		ui.AddSpacing();
		//ui.AddMinimizerToggle();
		SurfingGuiTypes st = OFX_IM_TOGGLE_ROUNDED;
		ui.Add(bGui, st);
		ui.Add(bGui_GptConversation, st);
		ui.Add(bigTextInput.bGui, st);
		ofxImGuiSurfing::AddToggleNamed(bBottomTextInput, "Bottom", "Top");
#ifdef USE_PRESETS
		ui.Add(presets.bGui, st);
#endif

		// 3
		ui.NextColumn();
		ui.AddLabelBig("PRESETS");
		ui.AddSpacing();
#ifdef USE_PRESETS
		presets.drawImGuiClicker(false, true);
#endif

		//TODO:
		ui.Add(colorBg, OFX_IM_COLOR_BOX_FULL_WIDTH_NO_ALPHA);
		ui.Add(colorAccent, OFX_IM_COLOR_BOX_FULL_WIDTH);
		ui.Add(colorAssistant, OFX_IM_COLOR_BOX_FULL_WIDTH);
		ui.Add(colorUser, OFX_IM_COLOR_BOX_FULL_WIDTH);

		ui.EndColumns();

		ui.AddSpacingBigSeparated();

		ui.BeginColumns(3, "##more2");
		ui.AddLabelBig("GPT");
		ui.AddSpacing();
		ui.Add(amountResultsPrompt, OFX_IM_STEPPER);
		ui.AddTooltip("List Size");
		if (ImGui::Button("Gpt Restart")) doGptRestart();
		ui.AddCombo(indexModel, modelsNames);
		ui.AddSpacing();

		ui.NextColumn();
		ui.AddLabelBig("ELEVEN-LABS");
		ui.AddSpacing();

		ui.EndColumns();

		//ui.Unindent();

		ui.EndTree();
	}

	ui.AddSpacing();

#if 0
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Custom", NULL, locationWindowContext == -1)) locationWindowContext = -1;
		if (ImGui::MenuItem("Center", NULL, locationWindowContext == -2)) locationWindowContext = -2;
		if (ImGui::MenuItem("Top-left", NULL, locationWindowContext == 0)) locationWindowContext = 0;
		if (ImGui::MenuItem("Top-right", NULL, locationWindowContext == 1)) locationWindowContext = 1;
		if (ImGui::MenuItem("Bottom-left", NULL, locationWindowContext == 2)) locationWindowContext = 2;
		if (ImGui::MenuItem("Bottom-right", NULL, locationWindowContext == 3)) locationWindowContext = 3;
		if (p_open && ImGui::MenuItem("Close"))
		{
			p_open = false;
			bGui_WindowContextMenu = false;
		}
		ImGui::EndPopup();
	}
#endif
		}


//#ifdef USE_OFX_ELEVEN_LABS
//	static auto drawElevenLabsExtras = [this]() {
//		ui.AddSpacing();
//		if (ui.AddButton("Reset##tts", OFX_IM_BUTTON)) {
//			tts.doReset();
//		}
//		ui.AddTooltip("Reset settings");
//
//		if (ui.AddButton("Replay##tts", OFX_IM_BUTTON, 2, 1)) {
//			tts.doReplayAudio();
//		}
//		ui.AddTooltip("Replay last audio");
//
//		if (ui.AddButton("Resend##tts", OFX_IM_BUTTON, 2)) {
//			tts.doResend();
//		}
//		ui.AddTooltip("Resend last text");
//
//		if (ui.AddButton("Cancel##tts", OFX_IM_BUTTON, 2, 1)) {
//			tts.doCancelRequest();
//		}
//		ui.AddTooltip("Force stop request");
//
//		if (ui.AddButton("Restart##tts", OFX_IM_BUTTON, 2)) {
//			tts.doRestart();
//		}
//		ui.AddTooltip("Restart connection");
//		};
//#endif
