#include <ncine/imgui.h>
#include <ncine/Application.h>
#include "singletons.h"
#include "gui/gui_labels.h"
#include "gui/gui_common.h"
#include "gui/RenderGuiWindow.h"
#include "gui/UserInterface.h"
#include "gui/FileDialog.h"
#include "Canvas.h"
#include "AnimationManager.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderGuiWindow::RenderGuiWindow(UserInterface &ui)
    : ui_(ui)
{
#ifdef __ANDROID__
	filename.assign("animation");
#endif
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

float RenderGuiWindow::resizeAmount(ResizeLevel rl)
{
	switch (rl)
	{
		case ResizeLevel::X1_8:
			return 0.125f;
		case ResizeLevel::X1_4:
			return 0.25f;
		case ResizeLevel::X1_2:
			return 0.5f;
		case ResizeLevel::X1:
			return 1.0f;
		case ResizeLevel::X2:
			return 2.0f;
		case ResizeLevel::X4:
			return 4.0f;
		case ResizeLevel::X8:
			return 8.0f;
	}
	return 1.0f;
}

float RenderGuiWindow::resizeAmount() const
{
	return resizeAmount(resizeLevel);
}

void RenderGuiWindow::setResize(float resizeAmount)
{
	if (resizeAmount <= 0.125f)
		resizeLevel = ResizeLevel::X1_8;
	else if (resizeAmount <= 0.25f)
		resizeLevel = ResizeLevel::X1_4;
	else if (resizeAmount <= 0.5f)
		resizeLevel = ResizeLevel::X1_2;
	else if (resizeAmount <= 1.0f)
		resizeLevel = ResizeLevel::X1;
	else if (resizeAmount <= 2.0f)
		resizeLevel = ResizeLevel::X2;
	else if (resizeAmount <= 4.0f)
		resizeLevel = ResizeLevel::X4;
	else
		resizeLevel = ResizeLevel::X8;
}

void RenderGuiWindow::create()
{
	if (ImGui::Begin(Labels::Render, nullptr))
	{
		if (directory.isEmpty())
		{
#ifdef __ANDROID__
			directory.assign(ui::androidSaveDir.data());
#else
			directory.assign(nc::fs::currentDir());
#endif
		}
		if (shouldSaveFrames_ == false && shouldSaveSpritesheet_ == false)
		{
			ui::auxString.format("Save to: %s%s", Labels::FileDialog_SelectDirIcon, directory.data());
			if (ImGui::Button(ui::auxString.data()))
			{
				FileDialog::config.directory = directory;
				FileDialog::config.windowIcon = Labels::FileDialog_SelectDirIcon;
				FileDialog::config.windowTitle = "Render save directory";
				FileDialog::config.okButton = Labels::Ok;
				FileDialog::config.selectionType = FileDialog::SelectionType::DIRECTORY;
				FileDialog::config.extensions = nullptr;
				FileDialog::config.action = FileDialog::Action::RENDER_DIR;
				FileDialog::config.windowOpen = true;
			}
		}
		else
			ImGui::Text("%s", directory.data());

		int inputTextFlags = ImGuiInputTextFlags_CallbackResize;
		if (shouldSaveFrames_ || shouldSaveSpritesheet_)
			inputTextFlags |= ImGuiInputTextFlags_ReadOnly;
		ImGui::InputText("Filename prefix", filename.data(), ui::MaxStringLength,
		                 inputTextFlags, ui::inputTextCallback, &filename);

		currentComboResize_ = static_cast<int>(resizeLevel);
		ImGui::Combo("Resize Level", &currentComboResize_, ResizeStrings, IM_ARRAYSIZE(ResizeStrings));

		resizeLevel = static_cast<RenderGuiWindow::ResizeLevel>(currentComboResize_);
		saveAnimStatus_.canvasResize = resizeAmount();

		ImGui::InputInt("FPS", &saveAnimStatus_.fps);
		ImGui::SliderInt("Num Frames", &saveAnimStatus_.numFrames, 1, 10 * saveAnimStatus_.fps); // Hard-coded limit
		float duration = saveAnimStatus_.numFrames * saveAnimStatus_.inverseFps();
		ImGui::SliderFloat("Duration", &duration, 0.0f, 10.0f, "%.3fs"); // Hard-coded limit

		const nc::Vector2i uncappedFrameSize(theCanvas->texWidth() * saveAnimStatus_.canvasResize, theCanvas->texHeight() * saveAnimStatus_.canvasResize);
		// Immediately-invoked function expression for const initialization
		const nc::Vector2i frameSize = [&] {
			nc::Vector2i size = uncappedFrameSize;
			size.x = (size.x > theCanvas->maxTextureSize()) ? theCanvas->maxTextureSize() : size.x;
			size.y = (size.y > theCanvas->maxTextureSize()) ? theCanvas->maxTextureSize() : size.y;
			return size;
		}();

		ui::auxString.format("%d x %d", frameSize.x, frameSize.y);
		if (uncappedFrameSize.x > frameSize.x || uncappedFrameSize.y > frameSize.y)
			ui::auxString.formatAppend(" (capped from %d x %d)", uncappedFrameSize.x, uncappedFrameSize.y);
		ImGui::Text("Frame size: %s", ui::auxString.data());

		const int sideX = static_cast<int>(ceil(sqrt(saveAnimStatus_.numFrames)));
		const int sideY = (sideX * (sideX - 1) > saveAnimStatus_.numFrames) ? sideX - 1 : sideX;
		const nc::Vector2i uncappedSpritesheetSize(sideX * frameSize.x, sideY * frameSize.y);
		// Immediately-invoked function expression for const initialization
		const nc::Vector2i spritesheetSize = [&] {
			nc::Vector2i size = uncappedSpritesheetSize;
			size.x = (size.x > theCanvas->maxTextureSize()) ? theCanvas->maxTextureSize() : size.x;
			size.y = (size.y > theCanvas->maxTextureSize()) ? theCanvas->maxTextureSize() : size.y;
			return size;
		}();

		ui::auxString.format("%d x %d", spritesheetSize.x, spritesheetSize.y);
		if (uncappedSpritesheetSize.x > spritesheetSize.x || uncappedSpritesheetSize.y > spritesheetSize.y)
			ui::auxString.formatAppend(" (capped from %d x %d)", uncappedSpritesheetSize.x, uncappedSpritesheetSize.y);
		ImGui::Text("Spritesheet size: %s", ui::auxString.data());

		saveAnimStatus_.numFrames = static_cast<int>(duration * saveAnimStatus_.fps);
		if (saveAnimStatus_.numFrames < 1)
			saveAnimStatus_.numFrames = 1;

		if (shouldSaveFrames_ || shouldSaveSpritesheet_)
		{
			const unsigned int numSavedFrames = saveAnimStatus_.numSavedFrames;
			const float fraction = numSavedFrames / static_cast<float>(saveAnimStatus_.numFrames);
			ui::auxString.format("Frame: %d/%d", numSavedFrames, saveAnimStatus_.numFrames);
			ImGui::ProgressBar(fraction, ImVec2(0.0f, 0.0f), ui::auxString.data());
			ImGui::SameLine();
			if (ImGui::Button(Labels::Cancel))
				cancelRender();
		}
		else
		{
			if (ImGui::Button(Labels::SaveFrames))
			{
				if (filename.isEmpty())
					ui_.pushStatusErrorMessage("Set a filename prefix before saving an animation");
				else
				{
#ifdef DEMO_VERSION
					ui_.pushStatusInfoMessage("Saving frames is not possible in the demo version");
#else
					theAnimMgr->play();
					saveAnimStatus_.filename.format("%s_%03d.png", nc::fs::joinPath(directory, filename).data(), saveAnimStatus_.numSavedFrames);
					shouldSaveFrames_ = true;
					theResizedCanvas->resizeTexture(frameSize);
					// Disabling V-Sync for faster render times
					nc::theApplication().gfxDevice().setSwapInterval(0);
#endif
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(Labels::SaveSpritesheet))
			{
				if (filename.isEmpty())
					ui_.pushStatusErrorMessage("Set a filename prefix before saving an animation");
				else
				{
#ifdef DEMO_VERSION
					ui_.pushStatusInfoMessage("Saving frames is not possible in the demo version");
#else
					theAnimMgr->play();
					saveAnimStatus_.filename.format("%s.png", nc::fs::joinPath(directory, filename).data(), saveAnimStatus_.numSavedFrames);
					shouldSaveSpritesheet_ = true;
					saveAnimStatus_.sheetDestPos.set(0, 0);
					theResizedCanvas->resizeTexture(theCanvas->size() * saveAnimStatus_.canvasResize);
					theSpritesheet->resizeTexture(spritesheetSize);
					// Disabling V-Sync for faster render times
					nc::theApplication().gfxDevice().setSwapInterval(0);
#endif
				}
			}
		}
		ImGui::End();
	}
}

void RenderGuiWindow::signalFrameSaved()
{
	ASSERT(shouldSaveFrames_ || shouldSaveSpritesheet_);

	saveAnimStatus_.numSavedFrames++;
	if (shouldSaveFrames_)
		saveAnimStatus_.filename.format("%s_%03d.png", nc::fs::joinPath(directory, filename).data(), saveAnimStatus_.numSavedFrames);
	else if (shouldSaveSpritesheet_)
	{
		Canvas &sourceCanvas = (saveAnimStatus_.canvasResize != 1.0f) ? *theResizedCanvas : *theCanvas;
		saveAnimStatus_.sheetDestPos.x += sourceCanvas.texWidth();
		if (saveAnimStatus_.sheetDestPos.x + sourceCanvas.texWidth() > theSpritesheet->texWidth())
		{
			saveAnimStatus_.sheetDestPos.x = 0;
			saveAnimStatus_.sheetDestPos.y += sourceCanvas.texHeight();
		}
	}
	if (saveAnimStatus_.numSavedFrames == saveAnimStatus_.numFrames)
	{
		shouldSaveFrames_ = false;
		shouldSaveSpritesheet_ = false;
		saveAnimStatus_.numSavedFrames = 0;
		ui_.pushStatusInfoMessage("Animation saved");

		// Re-enabling V-Sync if it was enabled in the configuration
		if (theCfg.withVSync)
			nc::theApplication().gfxDevice().setSwapInterval(1);
	}
}

void RenderGuiWindow::cancelRender()
{
	if (shouldSaveFrames_ || shouldSaveSpritesheet_)
	{
		if (shouldSaveFrames_)
			ui::auxString.format("Render cancelled, saved %d out of %d frames", saveAnimStatus_.numSavedFrames, saveAnimStatus_.numFrames);
		else if (shouldSaveSpritesheet_)
			ui::auxString = "Render cancelled, the spritesheet has not been saved";
		ui_.pushStatusInfoMessage(ui::auxString.data());
		saveAnimStatus_.numSavedFrames = 0;
		shouldSaveFrames_ = false;
		shouldSaveSpritesheet_ = false;

		// Re-enabling V-Sync if it was enabled in the configuration
		if (theCfg.withVSync)
			nc::theApplication().gfxDevice().setSwapInterval(1);
	}
}
