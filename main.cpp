#include "openvr.h"

#include <atomic>
#include <iostream>
#include <string>
#include <sstream>

#include <thread>
#include <mutex>

#include <filesystem>
#include <vector>

int main(int argc, char* argv[]) {
	vr::EVRInitError err = vr::VRInitError_None;
	vr::VR_Init(&err, vr::VRApplication_Scene);

	if (err != vr::VRInitError_None) {
		std::cout << "failed to init openvr! Err: " << vr::VR_GetVRInitErrorAsEnglishDescription(err) << std::endl;
		return 1;
	}

	std::filesystem::path cwd = std::filesystem::current_path() / "actions.json";

	vr::EVRInputError inputError;
	inputError = vr::VRInput()->SetActionManifestPath(cwd.string().c_str());

	if (inputError != vr::VRInputError_None) {
		std::cout << "action manifest error " << inputError << std::endl;
	}

	vr::VRActionSetHandle_t actionSet;
	vr::VRInput()->GetActionSetHandle("/actions/default", &actionSet);

	vr::VRActionHandle_t handleSkeletonLeft;
	vr::VRInput()->GetActionHandle("/actions/default/in/skeleton_left", &handleSkeletonLeft);

	while (true) {
		vr::VREvent_t vEvent;
		while (vr::VRSystem()->PollNextEvent(&vEvent, sizeof(vr::VREvent_t))) {
			switch (vEvent.eventType)
			{
			case vr::EVREventType::VREvent_Quit:
			{
				return 0;
			}
			}
		}

		vr::VRActiveActionSet_t activeActionSet = { 0 };
		activeActionSet.ulActionSet = actionSet;
		if (vr::EVRInputError err = vr::VRInput()->UpdateActionState(&activeActionSet, sizeof(activeActionSet), 1)) {
			std::cout << "UpdateActionState error: " << err << std::endl;
		}

		vr::VRSkeletalSummaryData_t summaryData{};
		if (vr::EVRInputError err = vr::VRInput()->GetSkeletalSummaryData(handleSkeletonLeft, vr::VRSummaryType_FromDevice, &summaryData))
		{
			std::cout << "GetSkeletalSummaryData error: " << err << std::endl;
		}

		std::cout << "Curls: " << std::fixed << std::setprecision(3) << summaryData.flFingerCurl[0] << " " << summaryData.flFingerCurl[1] << " " << summaryData.flFingerCurl[2] << " " << summaryData.flFingerCurl[3] << " " << summaryData.flFingerCurl[4] << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	return 0;
}
