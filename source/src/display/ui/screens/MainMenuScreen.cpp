#include "MainMenuScreen.h"
#include "drivermanager.h"
#include "turbo.h"
#include "AnimationStorage.hpp"
#include "hardware/watchdog.h"
#include "focus_mode.h"
#include "neopicoleds.h"
#include "Chase.hpp"

#include "hardware/structs/resets.h"

extern uint32_t getMillis();
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define TURBO_SHOT_MIN 2
#define TURBO_SHOT_MAX 30
#define BRIGHTNESS_MAX 5

#define AIRCR_Register (*((volatile uint32_t *)(PPB_BASE + 0x0ED0C)))

bool MainMenuScreen::reqReboot = false;

void MainMenuScreen::gotoMainPage()
{
    menuPageIndex = Main_Page;
    menuItemIndex = 0;
    top = 0;
    bottom = MAX_LINES;
}

void MainMenuScreen::gotoExitSavePage()
{
    menuPageIndex = Save_Exit_Page; // go to SAVE AND EXIT
    menuItemIndex = 7;
    top = 0;
    bottom = 2;
}

void MainMenuScreen::init()
{

    getRenderer()->clearScreen();
    gotoMainPage();
    m1ButtonValue = Storage::getInstance().getProfilePinMappings()[7].action;
    m2ButtonValue = Storage::getInstance().getProfilePinMappings()[3].action;
    if (m2ButtonValue >= 24)
    {
        m2ButtonValue -= 10;
    }
    if (m1ButtonValue >= 24)
    {
        m1ButtonValue -= 10;
    }

    Storage::getInstance().getAddonOptions().focusModeOptions.enabled = true;
    shotCount = Storage::getInstance().getAddonOptions().turboOptions.shotCount;
    // AnimationStation::ConfigureBrightness(150, 1);
    const FocusModeOptions &options = Storage::getInstance().getAddonOptions().focusModeOptions;
    profileIndex = getGamepad()->getOptions().profileNumber;
    focusButtonList.reserve(18);
    for (uint8_t i = 0; i < focusButtonValuePage.size(); i++)
    {

        if (focusButtonValuePage[i].first & options.buttonLockMask)
        {
            focusButtonList.push_back(focusButtonValuePage[i]);
        }
    }
}

void MainMenuScreen::input_select()
{
    getRenderer()->clearScreen();
    getRenderer()->drawText(1, 1, "       Input Select");
}

void MainMenuScreen::shutdown()
{
    clearElements();
}

void MainMenuScreen::drawScreen()
{
    uint8_t i, position, pageSize;
    position = constrain(menuItemIndex - top, 0, MAX_LINES - 1);
    pageSize = getPageSize(menuPageIndex);
    switch (menuPageIndex)
    {
    case Main_Page: // main menu
        getRenderer()->drawText(4, 0, "  Set Menu");
        for (i = top; i < bottom; i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, mainMenu[i]);
        }
        getRenderer()->drawText(1, 1 + position, ">");

        break;
    case Input_Page: // input mode page
        getRenderer()->drawText(4, 0, "  Input Mode: ");
        for (i = top; i < bottom; i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, inputModePage[i]);
        }
        getRenderer()->drawText(1, 1 + position, "\x81");
        break;

    case Dpad_Page:
        getRenderer()->drawText(0, 0, "  Choose Dpad Mode: ");
        for (i = top; i < bottom; i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, dpadModePage[i]);
        }
        getRenderer()->drawText(1, 1 + position, "\x81");
        break;

    case SOCD_Page:
        getRenderer()->drawText(0, 0, "  Choose Socd Mode");
        for (i = top; i < bottom; i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, socdModePage[i]);
        }
        getRenderer()->drawText(1, 1 + position, "\x81");
        break;

    case Turbo_Page:
        getRenderer()->drawText(0, 3, "  Turbo Speed: " + std::to_string(menuItemIndex));
        getRenderer()->drawRectangle(10, 35, (menuItemIndex * 108) / 30 + 10, 45, true, true);
        break;

    case Profile_Page:
        getRenderer()->drawText(0, 3, "   Set Profile #: " + std::to_string(menuItemIndex));
        break;

    case RGB_Page:
    {
        uint8_t baseAniIndex = AnimationStation::options.baseAnimationIndex;
        getRenderer()->drawText(0, 0, "      RGB LED set");
        for (i = top; i < 2; i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, rgbLEDPage[i]);
        }
        switch (baseAniIndex)
        {
        case 0 /* constant-expression */:
            /* code */
            getRenderer()->drawText(3, (i - top) + 1, "-Static Color Set.");
            i++;
            break;
        case 1:
            getRenderer()->drawText(3, (i - top) + 1, "-Rainbow Speed Set.");
            i++;
            break;
        case 2:
            getRenderer()->drawText(3, (i - top) + 1, "-Chase Speed Set.");
            i++;
            break;
        case 3:
            getRenderer()->drawText(3, (i - top) + 1, "-Static Theme Set.");
            i++;
            break;
        case 4:
            getRenderer()->drawText(3, (i - top) + 1, "-Custom Theme Set.");
            i++;
            break;
        default:
            break;
        };
        getRenderer()->drawText(3, (bottom - top), "BACK");

        getRenderer()->drawText(1, 1 + position, ">");
    }
    break;
    case RGB_LED_Animation_Page:
        getRenderer()->drawText(0, 0, "  RGB LED Animation");
        for (i = top; i < bottom; i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, animationLEDPage[i]);
        }
        getRenderer()->drawText(1, 1 + position, "\x81");
        break;
    case RGB_LED_Brightness_Page:
        getRenderer()->drawText(0, 3, "   Brightness: " + std::to_string(AnimationStation::options.brightness));
        getRenderer()->drawRectangle(10, 35, (menuItemIndex * 108) / 5 + 10, 45, true, true);
        break;
    case RGB_LED_Chase_Speed_Page:
        getRenderer()->drawText(0, 3, "  Chase Speed: " + std::to_string(menuItemIndex) + " ");
        getRenderer()->drawRectangle(10, 35, (menuItemIndex * 108) / 20 + 10, 45, true, true);
        break;
    case RGB_LED_Rainbow_Cycle_Speed_Page:
        getRenderer()->drawText(0, 3, "  Rainbow Speed: " + std::to_string(menuItemIndex) + " ");
        getRenderer()->drawRectangle(10, 35, (menuItemIndex * 108) / 20 + 10, 45, true, true);
        break;
    case RGB_LED_Static_Theme_Page:
        getRenderer()->drawText(0, 3, "  Static Theme: " + std::to_string(menuItemIndex) + " ");
        getRenderer()->drawRectangle(10, 35, (menuItemIndex * 108) / (StaticTheme::themes.size()) + 10, 45, true, true);
        break;
    case RGB_LED_StaticColor_Page:
        getRenderer()->drawText(0, 0, "  LED Static Color");
        for (i = top; i < bottom; i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, staticColorLEDPage[i]);
        }
        getRenderer()->drawText(1, 1 + position, "\x81");
        break;
    case MButton_Page:
    {
        int8_t m1index = m1ButtonValue;
        int8_t m2index = m2ButtonValue;
        getRenderer()->drawText(0, 0, "      M1/M2 Set");
        if (m1index < 0)
        {
            m1index = 0;
        }
        if (m2index < 0)
        {
            m2index = 0;
        }
        if (m1index >= 34)
        {
            m1index -= 10;
        }
        if (m2index >= 34)
        {
            m2index -= 10;
        }
        getRenderer()->drawText(3, 1, mButtonPage[0] + buttonValuePage[m1index]); // M1:
        getRenderer()->drawText(3, 2, mButtonPage[1] + buttonValuePage[m2index]); // M2:
        getRenderer()->drawText(3, 3, mButtonPage[2]);                            // Back
        getRenderer()->drawText(1, 1 + position, "\x81");
    }
    break;
    case BValue_Page:
        getRenderer()->drawText(0, 0, "  GP2040|XINPUT|PS");
        for (i = top; i < bottom; i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, buttonValuePage[i]);
        }
        getRenderer()->drawText(1, 1 + position, "\x81");

        break;
    case Focus_Mode_Page:
    {
        std::string isLockButton = Storage::getInstance().getAddonOptions().focusModeOptions.buttonLockEnabled ? "ON" : "OFF";
        std::string isMacroButton = Storage::getInstance().getAddonOptions().focusModeOptions.macroLockEnabled ? "ON" : "OFF";
        getRenderer()->drawText(0, 0, "  Focus Mode set.");
        // getRenderer()->drawText(3, 1, focusModePage[0] + (Storage::getInstance().getAddonOptions().focusModeOptions.enabled ? "ON" : "OFF")); // Lock Button:
        if (Storage::getInstance().getAddonOptions().focusModeOptions.enabled)
        {
            bottom = focusModePage.size();

            getRenderer()->drawText(3, 1, focusModePage[0] + isLockButton);  // Lock Button:
            getRenderer()->drawText(3, 2, focusModePage[1]);                 // Lock buttons set.
            getRenderer()->drawText(3, 3, focusModePage[2] + isMacroButton); // Lock Macro:
            getRenderer()->drawText(3, 4, focusModePage[3]);                 // Back
        }
        else
        {
            getRenderer()->drawText(3, 2, focusModePage[4]); // Back
            bottom = 2;
            pageSize = 2;
        }

        getRenderer()->drawText(1, 1 + position, ">");
    }
    break;
    case Edit_Focus_Buttons_Page:
    {
        uint8_t i = 0;
        getRenderer()->drawText(0, 0, "   LockButtons Set");
        for (i = top; i < bottom; i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, editFocusButtons[i]);
        }
        getRenderer()->drawText(1, 1 + position, "\x81");
    }
    break;
    case List_Focus_Button_Page:
    {
        getRenderer()->drawText(0, 0, "    LockButtons:");
        uint8_t i = 0;
        for (i = 0; i < focusButtonList.size(); i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, focusButtonList[i].second);
        }
        getRenderer()->drawText(1, 1 + position, "\x81");
    }
    break;
    case Add_Focus_Button_Page:
        getRenderer()->drawText(0, 0, "    Add Button");
        for (i = 0; i < focusButtonValuePage.size(); i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, focusButtonValuePage[i].second);
        }

        getRenderer()->drawText(1, 1 + position, "\x81");
        break;
    case Delete_Focus_Button_Page:
        getRenderer()->drawText(0, 0, "    Delete Button");
        for (i = 0; i < focusButtonList.size(); i++)
        {
            getRenderer()->drawText(3, (i - top) + 1, focusButtonList[i].second);
        }
        getRenderer()->drawText(1, 1 + position, "\x81");
        break;

    case Save_Exit_Page:
        getRenderer()->drawText(2, 3, " Save and Exit ?");
        getRenderer()->drawText(6, 5, "YES ");
        getRenderer()->drawText(12, 5, "NO ");
        getRenderer()->drawText(5 + position, 5, ">");
        break;
    default:
        break;
    }
    // print the scroll
    if (bottom < pageSize)
    {
        getRenderer()->drawText(19, MAX_LINES, "\x85");
    }
    if (top > 0)
    {
        getRenderer()->drawText(19, 1, "\x84");
    }
}

uint8_t MainMenuScreen::getPageSize(Page_Index idx)
{
    uint8_t pageSize;
    switch (idx)
    {
    case Main_Page:
        pageSize = mainMenu.size();
        break;
    case Input_Page:
        pageSize = inputModePage.size();
        break;
    case Dpad_Page:
        pageSize = dpadModePage.size();
        break;
    case SOCD_Page:
        pageSize = socdModePage.size();
        break;
    case RGB_Page:
        pageSize = rgbLEDPage.size();
        break;
    case RGB_LED_Animation_Page:
        pageSize = animationLEDPage.size();
        break;
    case RGB_LED_StaticColor_Page:
        pageSize = staticColorLEDPage.size();
        break;
    case MButton_Page:
        pageSize = mButtonPage.size();
        break;
    case BValue_Page:
        pageSize = buttonValuePage.size();
        break;
    case Focus_Mode_Page:
        pageSize = focusModePage.size();
        break;
    case List_Focus_Button_Page:
        pageSize = focusButtonList.size();
        break;
    case Edit_Focus_Buttons_Page:
        pageSize = editFocusButtons.size();
        break;
    case Add_Focus_Button_Page:
        pageSize = focusButtonValuePage.size();
        break;
    case Delete_Focus_Button_Page:
        pageSize = focusButtonList.size();
        break;
    default:
        pageSize = 0;
        break;
    }
    return pageSize;
}

void MainMenuScreen::updateSettings()
{
    Storage::getInstance().save();
}

void MainMenuScreen::setMenu(std::vector<MenuEntry> *menu)
{
    currentMenu = menu;
}

int8_t MainMenuScreen::update()
{
    uint8_t pageSize = 0;

    static int8_t buttonIndex = 255;
    uint32_t buttonState = getProcessedGamepad()->state.buttons;
    TurboOptions &options = Storage::getInstance().getAddonOptions().turboOptions;

    pageSize = getPageSize(menuPageIndex);

    if (prevButtonState && !buttonState)
    {
        switch (prevButtonState)
        {
        case (GAMEPAD_MASK_E2): // up
            switch (menuPageIndex)
            {
            case Turbo_Page:
            case Profile_Page:
            case RGB_LED_Chase_Speed_Page:
            case RGB_LED_Rainbow_Cycle_Speed_Page:
            case RGB_LED_Static_Theme_Page:
                /* code */
                break;
            // case Focus_Mode_Page:
            //     break;
            default:
                if (menuItemIndex > 0)
                {
                    menuItemIndex--;
                    if (menuItemIndex < top)
                    {
                        top--;
                        bottom--;
                    }
                }
                break;
            }
            break;
        case (GAMEPAD_MASK_E1): // down
            switch (menuPageIndex)
            {
            case Turbo_Page:
            case Profile_Page:
            case RGB_LED_Chase_Speed_Page:
            case RGB_LED_Rainbow_Cycle_Speed_Page:
            case RGB_LED_Static_Theme_Page:
                /* code */
                break;
            // case Focus_Mode_Page:
            //     break;
            default:

                if (menuPageIndex == Focus_Mode_Page && !Storage::getInstance().getAddonOptions().focusModeOptions.enabled)
                {
                    menuItemIndex = 0;
                    pageSize = 2;
                }

                if (menuItemIndex < pageSize - 1)
                {
                    menuItemIndex++;

                    // determine if cursor is at the bottom of the screen
                    if (menuItemIndex > MAX_LINES - 1 && bottom < pageSize)
                    {
                        /* code */
                        top++;
                        bottom++;
                    }
                }

                break;
            };

            break;
        case (GAMEPAD_MASK_E5):

            switch (menuPageIndex)
            {
            case Main_Page: // main menu page
                if (menuItemIndex == 0)
                {
                    menuPageIndex = Input_Page; // go to input mode
                    menuItemIndex = 0;          // reset index of the menu item.
                    top = 0;
                    bottom = inputModePage.size() > MAX_LINES ? MAX_LINES : inputModePage.size();
                }
                else if (menuItemIndex == 1)
                {
                    menuPageIndex = Dpad_Page; // go to input mode
                    menuItemIndex = 0;         // reset index of the menu item.
                    top = 0;
                    bottom = dpadModePage.size() > MAX_LINES ? MAX_LINES : dpadModePage.size();
                }
                else if (menuItemIndex == 2)
                {
                    menuPageIndex = SOCD_Page; // go to socd mode
                    menuItemIndex = 0;         // reset index of the menu item.
                    top = 0;
                    bottom = socdModePage.size() > MAX_LINES ? MAX_LINES : socdModePage.size();
                }
                else if (menuItemIndex == 3) // go to turbo MODE page
                {
                    menuPageIndex = Turbo_Page; // go to turbo mode
                    menuItemIndex = Storage::getInstance().getAddonOptions().turboOptions.shotCount;
                }
                else if (menuItemIndex == 4) // go to profile number page
                {
                    menuPageIndex = Profile_Page; // go to profile mode
                    menuItemIndex = profileIndex;
                }
                else if (menuItemIndex == 5) // go to RGB led page
                {
                    menuPageIndex = RGB_Page; // go to rgb led set mode
                    menuItemIndex = 0;        // reset index of the menu item.
                    top = 0;
                    bottom = rgbLEDPage.size() > MAX_LINES ? MAX_LINES : rgbLEDPage.size();
                }
                else if (menuItemIndex == 6) // go to M1/M2 Button setting page
                {
                    menuPageIndex = MButton_Page; //
                    menuItemIndex = 0;            // reset index of the menu item.
                    top = 0;
                    bottom = mButtonPage.size() > MAX_LINES ? MAX_LINES : mButtonPage.size();
                }
                else if (menuItemIndex == 7)
                {
                    menuPageIndex = Focus_Mode_Page; // go to focus mode
                    menuItemIndex = 0;               // reset index of the menu item.
                    top = 0;
                    bottom = focusModePage.size() > MAX_LINES ? MAX_LINES : focusModePage.size();
                }

                else if (menuItemIndex == 8) // save and exit Main Menu Page
                {
                    // return DisplayMode::BUTTONS;
                    gotoExitSavePage();
                }
                break;
            case Input_Page: // input page
            {
                uint8_t inputItem = menuItemIndex;
                if (inputItem < pageSize - 1)
                {

                    if (inputItem > 5)
                    {
                        /* code */
                        menuItemIndex--;
                    }

                    if (inputItem == 5)
                    {
                        Storage::getInstance().getGamepadOptions().inputMode = INPUT_MODE_PS5;
                    }
                    else if (Storage::getInstance().getGamepadOptions().inputMode != (InputMode)menuItemIndex)
                    {
                        Storage::getInstance().getGamepadOptions().inputMode = (InputMode)menuItemIndex;
                    }
                    // DriverManager::getInstance().setup((InputMode)menuItemIndex);
                    Gamepad::reqSave = true;
                    reqReboot = true;
                    gotoExitSavePage();
                }
                else
                {
                    menuPageIndex = Main_Page;
                    menuItemIndex = 0;
                    top = 0;
                    bottom = MAX_LINES;
                }
            }
            break;
            case Dpad_Page:                       // dpad page
                if (menuItemIndex < pageSize - 1) // BACK menu item
                {
                    Storage::getInstance().GetGamepad()->setDpadMode((DpadMode)menuItemIndex);
                    Gamepad::reqSave = true;
                }
                menuPageIndex = Main_Page;
                menuItemIndex = 1;
                top = 0;
                bottom = MAX_LINES;
                break;
            case SOCD_Page:                       // SOCD page
                if (menuItemIndex < pageSize - 1) // BACK menu item
                {
                    Storage::getInstance().GetGamepad()->setSOCDMode((SOCDMode)menuItemIndex);
                }
                menuPageIndex = Main_Page;
                menuItemIndex = 2;
                top = 0;
                bottom = MAX_LINES;
                break;
            case Turbo_Page: // Turbo page
                Storage::getInstance().getAddonOptions().turboOptions.shotCount = menuItemIndex+1;
                TurboInput::reqTurbe = true;
                menuPageIndex = Main_Page;
                menuItemIndex = 3;
                top = 0;
                bottom = MAX_LINES;

                break;
            case Profile_Page: // profile page
                if (Storage::getInstance().GetGamepad()->getOptions().profileNumber != (uint32_t)(menuItemIndex))
                {
                    Storage::getInstance().setProfile(menuItemIndex);
                    Gamepad::reqSave = true;
                    reqReboot = true;
                    sleep_ms(20);
                }
                gotoExitSavePage();
                break;
            case RGB_Page:                         // RGB LED setting page
                if (menuItemIndex == pageSize - 1) // BACK menu item
                {
                    Gamepad::reqSave = true;
                    menuPageIndex = Main_Page;
                    menuItemIndex = 5;
                    top = 0;
                    bottom = MAX_LINES;
                }
                else
                {
                    switch (menuItemIndex)
                    {
                    case 0: // animation LED set
                        menuPageIndex = RGB_LED_Animation_Page;
                        bottom = animationLEDPage.size() > MAX_LINES ? MAX_LINES : animationLEDPage.size();
                        menuItemIndex = 0;
                        break;
                    case 1: // brightness set
                        menuPageIndex = RGB_LED_Brightness_Page;
                        bottom = MAX_LINES;
                        menuItemIndex = (AnimationStation::GetBrightness());
                        break;
                    case 2: // static color set
                    {
                        uint8_t index = AnimationStation::options.baseAnimationIndex;
                        switch (index)
                        {
                        case 0 /* constant-expression */:
                            menuPageIndex = RGB_LED_StaticColor_Page;
                            bottom = staticColorLEDPage.size() > MAX_LINES ? MAX_LINES : staticColorLEDPage.size();
                            menuItemIndex = 0;
                            break;
                        case 1:
                            menuPageIndex = RGB_LED_Rainbow_Cycle_Speed_Page;
                            bottom = MAX_LINES;
                            menuItemIndex = (AnimationStation::GetCycleSpeed()) / RAINBOW_CYCLE_INCREMENT;
                            break;
                        case 2:
                            menuPageIndex = RGB_LED_Chase_Speed_Page;
                            bottom = MAX_LINES;
                            menuItemIndex = (AnimationStation::GetChaseSpeed()) / CHASE_CYCLE_INCREMENT;
                            break;
                        case 3:
                            menuPageIndex = RGB_LED_Static_Theme_Page;
                            bottom = MAX_LINES;
                            menuItemIndex = 20;
                            break;

                        default:
                            break;
                        }
                    }
                    break;
                    default:
                        break;
                    }
                    top = 0;
                }
                break;
            case RGB_LED_Animation_Page:
                if (menuItemIndex != pageSize - 1) // BACK menu item
                {
                    AnimationStation::options.baseAnimationIndex = menuItemIndex;
                    AnimationStation::as_traggle = true;
                }
                menuPageIndex = RGB_Page;
                menuItemIndex = 0;
                top = 0;
                bottom = rgbLEDPage.size() > MAX_LINES ? MAX_LINES : rgbLEDPage.size();
                break;
            case RGB_LED_Brightness_Page:
                menuPageIndex = RGB_Page;
                menuItemIndex = 1;
                top = 0;
                bottom = rgbLEDPage.size() > MAX_LINES ? MAX_LINES : rgbLEDPage.size();
                break;
            case RGB_LED_StaticColor_Page:
                if (menuItemIndex != pageSize - 1)
                {
                    AnimationStation::options.staticColorIndex = menuItemIndex;
                }
                menuPageIndex = RGB_Page;
                menuItemIndex = 2;
                top = 0;
                bottom = rgbLEDPage.size() > MAX_LINES ? MAX_LINES : rgbLEDPage.size();
                break;
            case RGB_LED_Chase_Speed_Page:
                menuPageIndex = RGB_Page;
                menuItemIndex = 2;
                top = 0;
                bottom = rgbLEDPage.size() > MAX_LINES ? MAX_LINES : rgbLEDPage.size();
                break;
            case RGB_LED_Rainbow_Cycle_Speed_Page:
                menuPageIndex = RGB_Page;
                menuItemIndex = 2;
                top = 0;
                bottom = rgbLEDPage.size() > MAX_LINES ? MAX_LINES : rgbLEDPage.size();
                break;
            case RGB_LED_Static_Theme_Page:
                menuPageIndex = RGB_Page;
                menuItemIndex = 2;
                top = 0;
                bottom = rgbLEDPage.size() > MAX_LINES ? MAX_LINES : rgbLEDPage.size();
                break;
            case MButton_Page:
                if (menuItemIndex == pageSize - 1) // BACK menu item
                {
                    menuPageIndex = Main_Page;
                    menuItemIndex = 2;

                    top = 0;
                    bottom = MAX_LINES + 1;
                    reqReboot = true;
                    Gamepad::reqSave = true;
                    /* code */
                }
                else
                {
                    buttonIndex = menuItemIndex;
                    menuPageIndex = BValue_Page;
                    menuItemIndex = 0;
                    top = 0;
                    bottom = buttonValuePage.size() > MAX_LINES ? MAX_LINES : buttonValuePage.size();
                }
                break;
            case BValue_Page: // input page
            {
                if (buttonIndex == 0)
                {
                    m1ButtonValue = menuItemIndex;
                    menuPageIndex = MButton_Page;
                    menuItemIndex = 0;
                    top = 0;
                    bottom = mButtonPage.size() > MAX_LINES ? MAX_LINES : mButtonPage.size();
                    if (m1ButtonValue < 1)
                    {
                        Storage::getInstance().getProfilePinMappings()[7].action = GpioAction::NONE;
                        if (profileIndex > 1)
                        {
                            /* code */
                            Storage::getInstance().getProfileOptions().gpioMappingsSets[profileIndex - 2].pins[7].action = GpioAction::NONE;
                        }
                        else
                        {
                            Storage::getInstance().getGpioMappings().pins[7].action = GpioAction::NONE;
                        }
                    }
                    else
                    {
                        if (m1ButtonValue >= 24)
                        {
                            m1ButtonValue += 10;
                        }

                        Storage::getInstance().getProfilePinMappings()[7].action = static_cast<GpioAction>(m1ButtonValue);
                        if (profileIndex > 1)
                        {
                            Storage::getInstance().getProfileOptions().gpioMappingsSets[profileIndex - 2].pins[7].action = (GpioAction)m1ButtonValue;
                            /* code */
                        }
                        else
                        {
                            Storage::getInstance().getGpioMappings().pins[7].action = (GpioAction)m1ButtonValue;
                        }
                    }

                    Gamepad::reqSave = true;
                }
                else if (buttonIndex == 1)
                {
                    m2ButtonValue = menuItemIndex;
                    menuPageIndex = MButton_Page;
                    menuItemIndex = 1;
                    top = 0;
                    bottom = mButtonPage.size() > MAX_LINES ? MAX_LINES : mButtonPage.size();
                    if (m2ButtonValue < 1)
                    {
                        Storage::getInstance().getProfilePinMappings()[3].action = GpioAction::NONE;
                        if (profileIndex > 1)
                        {
                            /* code */
                            Storage::getInstance().getProfileOptions().gpioMappingsSets[profileIndex - 2].pins[3].action = GpioAction::NONE;
                        }
                        else
                        {
                            Storage::getInstance().getGpioMappings().pins[3].action = GpioAction::NONE;
                        }
                    }
                    else
                    {
                        if (m2ButtonValue >= 24)
                        {
                            m2ButtonValue += 10;
                        }
                        Storage::getInstance().getProfilePinMappings()[3].action = static_cast<GpioAction>(m2ButtonValue);
                        if (profileIndex > 1)
                        {
                            /* code */
                            Storage::getInstance().getProfileOptions().gpioMappingsSets[profileIndex - 2].pins[3].action = (GpioAction)m2ButtonValue;
                        }
                        else
                        {
                            Storage::getInstance().getGpioMappings().pins[3].action = (GpioAction)m2ButtonValue;
                        }
                    }
                    Gamepad::reqSave = true;
                }

                break;
            }
            case Focus_Mode_Page: // focus mode page
                if (Storage::getInstance().getAddonOptions().focusModeOptions.enabled)
                {
                    if (menuItemIndex == focusModePage.size() - 1)
                    {
                        Gamepad::reqSave = true;
                        menuPageIndex = Main_Page;
                        menuItemIndex = 7;
                        top = 1;
                        bottom = MAX_LINES + 1;
                    }
                    else if (menuItemIndex == 0)
                    {
                        if (Storage::getInstance().getAddonOptions().focusModeOptions.buttonLockEnabled)
                        {
                            Storage::getInstance().getAddonOptions().focusModeOptions.buttonLockEnabled = false;
                        }
                        else
                        {
                            Storage::getInstance().getAddonOptions().focusModeOptions.buttonLockEnabled = true;
                        }
                    }
                    else if (menuItemIndex == 2)
                    {
                        if (Storage::getInstance().getAddonOptions().focusModeOptions.macroLockEnabled)
                        {
                            Storage::getInstance().getAddonOptions().focusModeOptions.macroLockEnabled = false;
                        }
                        else
                        {
                            Storage::getInstance().getAddonOptions().focusModeOptions.macroLockEnabled = true;
                        }
                    }
                    else if (menuItemIndex == 1)
                    {

                        menuPageIndex = Edit_Focus_Buttons_Page;
                        menuItemIndex = 0;
                        top = 0;
                        bottom = editFocusButtons.size() > MAX_LINES ? MAX_LINES : editFocusButtons.size();
                    }
                }
                else
                {
                    if (menuItemIndex == 1)
                    {
                        Gamepad::reqSave = true;
                        menuPageIndex = Main_Page;
                        menuItemIndex = 7;
                        top = 1;
                        bottom = MAX_LINES + 1;
                    }
                }

                break;
            case Edit_Focus_Buttons_Page:
            {
                // buttonIndex = 3;
                if (menuItemIndex == 1) // add focus button
                {
                    menuPageIndex = Add_Focus_Button_Page;
                    menuItemIndex = 0;
                    top = 0;
                    bottom = 18 > MAX_LINES ? MAX_LINES : 18;
                }
                else if (menuItemIndex == 2) // delete focus button
                {
                    menuPageIndex = Delete_Focus_Button_Page;
                    menuItemIndex = 0;
                    top = 0;
                    bottom = focusButtonList.size() > MAX_LINES ? MAX_LINES : focusButtonList.size();
                }
                else if (menuItemIndex == 0)
                {
                    menuPageIndex = List_Focus_Button_Page;
                    menuItemIndex = 0;
                    top = 0;
                    bottom = focusButtonList.size() > MAX_LINES ? MAX_LINES : focusButtonList.size();
                }
                else if (menuItemIndex == 3) // back
                {
                    menuPageIndex = Focus_Mode_Page;
                    menuItemIndex = 1;
                    top = 0;
                    bottom = MAX_LINES;
                }
            }
            break;
            case List_Focus_Button_Page:
            {

                menuPageIndex = Edit_Focus_Buttons_Page;
                menuItemIndex = 0;
                top = 0;
                bottom = editFocusButtons.size() > MAX_LINES ? MAX_LINES : editFocusButtons.size();
            }
            break;
            case Add_Focus_Button_Page:
            {

                uint32_t buttonMask = focusButtonValuePage[menuItemIndex].first;
                FocusModeOptions &options = Storage::getInstance().getAddonOptions().focusModeOptions;
                if (std::find(focusButtonList.begin(), focusButtonList.end(), focusButtonValuePage[menuItemIndex]) == focusButtonList.end())
                {
                    focusButtonList.push_back(focusButtonValuePage[menuItemIndex]);
                    options.buttonLockMask |= (int32_t)buttonMask;
                }
                menuPageIndex = Edit_Focus_Buttons_Page;
                menuItemIndex = 0;
                top = 0;
                bottom = editFocusButtons.size() > MAX_LINES ? MAX_LINES : editFocusButtons.size();
            }
            break;
            case Delete_Focus_Button_Page:
            {

                if (focusButtonList.size() > 0)
                {
                    uint32_t buttonMask = focusButtonList[menuItemIndex].first;
                    FocusModeOptions &options = Storage::getInstance().getAddonOptions().focusModeOptions;

                    options.buttonLockMask &= ~(int32_t)buttonMask;
                    focusButtonList.erase(focusButtonList.begin() + menuItemIndex);
                }

                menuPageIndex = Edit_Focus_Buttons_Page;
                menuItemIndex = 0;
                top = 0;
                bottom = editFocusButtons.size() > MAX_LINES ? MAX_LINES : editFocusButtons.size();
            }
            break;
            case Save_Exit_Page:
                if (menuItemIndex == 0)
                {
                    Gamepad::reqSave = true;
      
                    if (reqReboot)
                    {
                        watchdog_reboot(0, SRAM_END, 100); // software reboot
                    }

                    return DisplayMode::BUTTONS;
                }
                else
                {
                    gotoMainPage();
                }

                break;

            default:
                break;
            }
            break;
        case (GAMEPAD_MASK_E3): // Right
            switch (menuPageIndex)
            {
            case Main_Page:
                // return DisplayMode::BUTTONS;
                gotoExitSavePage();
                break;
            case Turbo_Page:
                if (menuItemIndex < TURBO_SHOT_MAX)
                {
                    menuItemIndex++;
                }
                break;
            case Profile_Page:
                if (menuItemIndex < 4)
                {
                    menuItemIndex++;
                }
                break;
            case RGB_LED_Brightness_Page:
                if (menuItemIndex < BRIGHTNESS_MAX)
                {
                    menuItemIndex++;
                    AnimationStation::IncreaseBrightness();
                }
                break;
            case RGB_LED_Chase_Speed_Page:
                if (menuItemIndex < 20)
                {
                    /* code */
                    menuItemIndex++;
                    AnimationStation::chaseCycleUp = true;
                }

                break;
            case RGB_LED_Rainbow_Cycle_Speed_Page:
                if (menuItemIndex < 20)
                {
                    /* code */
                    menuItemIndex++;
                    AnimationStation::rainbowSpeedUp = true;
                }

                break;
            case RGB_LED_Static_Theme_Page:
                if (menuItemIndex < StaticTheme::themes.size())
                {
                    /* code */
                    menuItemIndex++;
                    AnimationStation::staticThemeUp = true;
                }

                break;
            case Input_Page:
                break;
            case Add_Focus_Button_Page:
            case Delete_Focus_Button_Page:
                menuPageIndex = Edit_Focus_Buttons_Page;
                menuItemIndex = 0;
                top = 0;
                bottom = editFocusButtons.size() > MAX_LINES ? MAX_LINES : editFocusButtons.size();
                break;

            case Save_Exit_Page:
                if (menuItemIndex == 0)
                {
                    menuItemIndex = 7;
                }

                break;
            default:
                break;
            }
            break;
        case (GAMEPAD_MASK_E4): // left
            switch (menuPageIndex)
            {
            case Main_Page:
                // return DisplayMode::BUTTONS;
                gotoExitSavePage();
                break;
            case Input_Page:
            case Dpad_Page:
            case RGB_Page:
            case SOCD_Page:
            case MButton_Page:
            case Focus_Mode_Page:
                // gotoMainPage();
                break;
            case Turbo_Page:
                if (menuItemIndex > TURBO_SHOT_MIN)
                {
                    menuItemIndex--;
                }
                break;
            case Profile_Page:
                if (menuItemIndex > 1)
                {
                    menuItemIndex--;
                }
                break;
            case RGB_LED_Brightness_Page:
                if (menuItemIndex > 0)
                {
                    menuItemIndex--;
                    AnimationStation::DecreaseBrightness();
                    Gamepad::reqSave = true;
                }
                break;
            case RGB_LED_Chase_Speed_Page:
                if (menuItemIndex > CHASE_CYCLE_MIN / CHASE_CYCLE_INCREMENT)
                {
                    /* code */
                    menuItemIndex--;
                    AnimationStation::chaseCycleDown = true;
                }
                break;
            case RGB_LED_Rainbow_Cycle_Speed_Page:
                if (menuItemIndex > CHASE_CYCLE_MIN / CHASE_CYCLE_INCREMENT)
                {
                    /* code */
                    menuItemIndex--;
                    AnimationStation::rainbowSpeedDown = true;
                }
                break;
            case RGB_LED_Static_Theme_Page:
                if (menuItemIndex > 1)
                {
                    /* code */
                    menuItemIndex--;
                    AnimationStation::staticThemeDown = true;
                }

                break;
            case BValue_Page:
                menuPageIndex = MButton_Page;
                menuItemIndex = 0;
                top = 0;
                bottom = mButtonPage.size() > MAX_LINES ? MAX_LINES : mButtonPage.size();
                break;
            case Add_Focus_Button_Page:
            case Delete_Focus_Button_Page:
                menuPageIndex = Edit_Focus_Buttons_Page;
                menuItemIndex = 0;
                top = 0;
                bottom = editFocusButtons.size() > MAX_LINES ? MAX_LINES : editFocusButtons.size();
                break;
            case Save_Exit_Page:
                if (menuItemIndex == 7)
                {
                    menuItemIndex = 0;
                }

                break;
            default:
                break;
            }
        default:
            // prevDisplayMode = DisplayMode::CONFIG_INSTRUCTION;
            break;
        }
    }

    prevButtonState = buttonState;

    return -1;
}

/*
    void testMenu();

    std::vector<MenuEntry> mainMenu = {
        {"Menu 1", NULL, std::bind(&DisplayAddon::testMenu, this)},
        {"Menu 2", NULL, std::bind(&DisplayAddon::testMenu, this)},
        {"Menu 3", NULL, std::bind(&DisplayAddon::testMenu, this)},
        {"Menu 4", NULL, std::bind(&DisplayAddon::testMenu, this)},
        {"Menu 5", NULL, std::bind(&DisplayAddon::testMenu, this)},
        {"Menu 6", NULL, std::bind(&DisplayAddon::testMenu, this)},
        {"Menu 7", NULL, std::bind(&DisplayAddon::testMenu, this)},
        {"Menu 8", NULL, std::bind(&DisplayAddon::testMenu, this)},
    };

    std::vector<MenuEntry>* currentMenu = &mainMenu;
*/