#ifndef _MAINMENUSCREEN_H_
#define _MAINMENUSCREEN_H_

#include "GPGFX_UI_widgets.h"
#include "GPGFX_UI_types.h"
#include "storagemanager.h"
class MainMenuScreen : public GPScreen
{
private:
    enum Page_Index
    {
        Main_Page = 0,
        Input_Page = 1,
        Dpad_Page = 2,
        SOCD_Page = 3,
        Turbo_Page = 4,
        Profile_Page = 5,
        RGB_Page = 6,
        RGB_LED_Animation_Page = 7,
        RGB_LED_Brightness_Page = 8,
        RGB_LED_StaticColor_Page = 9,
        MButton_Page = 10,
        BValue_Page = 11,
        Save_Exit_Page = 12,
        Focus_Mode_Page = 13,
        Focus_Button_Page = 14,
        Add_Focus_Button_Page = 15,
        Delete_Focus_Button_Page = 16,
        RGB_LED_Chase_Speed_Page = 17,
        RGB_LED_Rainbow_Cycle_Speed_Page = 18,
        RGB_LED_Static_Theme_Page = 19,
        Edit_Focus_Buttons_Page = 20,
        List_Focus_Button_Page =21,
    };

public:
    MainMenuScreen() {}
    MainMenuScreen(GPGFX *renderer) { setRenderer(renderer); }
    void setMenu(std::vector<MenuEntry> *menu);
    virtual int8_t update();
    virtual void init();
    virtual void shutdown();
    void input_select();
    void dpad_select();
    void gotoMainPage();
    void gotoExitSavePage();
    void inputSlected();
    void updateSettings();
    uint8_t const MAX_LINES = 7;
    uint8_t getPageSize(Page_Index idx);

    static bool reqReboot;

protected:
    virtual void drawScreen();

private:
    Page_Index menuPageIndex;
    uint8_t menuItemIndex = 0;
    bool isPressed = false;
    uint8_t bottom = 7;
    uint8_t top = 1;
    uint8_t cursorPosition = 0;
    int8_t m1ButtonValue = 0;
    int8_t m2ButtonValue = 0;
    uint8_t parentPage = 0;
    uint8_t shotCount;
    uint32_t profileIndex = 0;
    uint8_t pre_index = 0;
    std::string currentMenuName;
    uint32_t checkDebounce;
    std::vector<MenuEntry> *currentMenu;
    uint32_t prevButtonState = 0;
    uint32_t ledMaxBrigness = 225;
    std::vector<std::pair<uint32_t, std::string>> focusButtonList;

    std::vector<std::string> mainMenu = {
        {"Input Mode Set."},
        {"Dpad Mode Set."},
        {"SOCD Mode Set."},
        {"Turbo Speed Set."},
        {"Profile # Set."},
        {"RGB LED Set."},
        {"M1/2 Button Set."},
        {"Focus Mode Set."},
        {"Save and Exit "},
    };

    std::vector<std::string> inputModePage = {

        {"XINPUT"},
        {"SWITCH"},
        {"PS3"},
        {"KEYBOARD"},
        {"PS4"},
        {"PS5"},
        {"XBOX ONE"},
        {"MDMINI"},
        {"NGMINI"},
        {"PCEMINI"},
        {"EGRET"},
        {"ASTRO"},
        {"PSCLASSIC"},
        {"XBOXORIGINAL"},

        // {"CONFIG"},
        // {"DINPUT/HID"},
        {"BACK"}

    };
    std::vector<std::string> dpadModePage = {

        {"Digital"},
        {"LEFT Analog"},
        {"RIGHT Analog"},
        {"BACK"},
    };

    std::vector<std::string> socdModePage = {

        {"UP Priority"},
        {"Neurtal"},
        {"Last Win"},
        {"First Win"},
        {"BYPASS"},
        {"BACK"},
    };

    std::vector<std::string> rgbLEDPage = {
        {"Animation Set."},
        {"Brightness Set."},
        {"-Static Color Set."},
        {"BACK"}};

    std::vector<std::string> animationLEDPage = {
        {"Static Color"},
        {"Rainbow Cycle"},
        {"Rainbow Chase"},
        {"Static Theme"},
        {"Custom Theme"},
        {"BACK"},
    };
    std::vector<std::string> staticColorLEDPage = {
        {"Static Black"},
        {"Static White"},
        {"Static Red"},
        {"Static Orange"},
        {"Static Yellow"},
        {"Static LimeGreen"},
        {"Static Green"},
        {"Static Seafoam"},
        {"Static Aqua"},
        {"Static SkyBlue"},
        {"Static Blue"},
        {"Static Purple"},
        {"Static Pink"},
        {"Static Magenta"},
        {"BACK"}

    };

    std::vector<std::string> mButtonPage = {
        {"M1: "},
        {"M2: "},
        {"BACK"}

    };
    std::vector<std::string> focusModePage = {
        {"LockButton: "},
        {"LockButtons Set."},
        {"LockMacro: "},
        {"BACK"}

    };

    std::vector<std::string> editFocusButtons = {
        {"List LockButtons."},
        {"Add..."},
        {"Delete..."},
        {"BACK"}
    };

    std::vector<std::pair<uint32_t, std::string>> focusButtonValuePage = {

        {GAMEPAD_MASK_B1, "B1 A|CROSS"},
        {GAMEPAD_MASK_B2, "B2 B|CIRCLE"},
        {GAMEPAD_MASK_B3, "B3 X|SQUARE"},
        {GAMEPAD_MASK_B4, "B4 Y|TRIANGLE"},
        {GAMEPAD_MASK_L1, "L1 LB|L1"},
        {GAMEPAD_MASK_R1, "R1 RB|R1"},
        {GAMEPAD_MASK_L2, "L2 LT|L2"},
        {GAMEPAD_MASK_R2, "R2 RT|R2"},
        {GAMEPAD_MASK_S1, "S1 SELECT"},
        {GAMEPAD_MASK_S2, "S2 START"},
        {GAMEPAD_MASK_A1, "A1 HOME|PS"},
        {GAMEPAD_MASK_A2, "A2 ~/CAPTURE"},
     
    };

    std::vector<std::string> buttonValuePage = {

        {"NONE"},
        {"UP"},
        {"DOWN"},
        {"LEFT"},
        {"RIGHT"},
        {"B1 A|CROSS"},
        {"B2 B|CIRCLE"},
        {"B3 X|SQUARE"},
        {"B4 Y|TRIANGLE"},
        {"L1 LB|L1"},
        {"R1 RB|R1"},
        {"L2 LT|L2"},
        {"R2 RT|R2"},
        {"SELECT"},
        {"START"},
        {"A1 HOME|PS"},
        {"A2 ~/CAPTURE"},
        {"L3 LS|L3"},
        {"R3 RS|R3"},
        {"FN"},
        {"DDI_UP"},
        {"DDI_DOWN"},
        {"DDI_LEFT"},
        {"DDI_RIGHT"},
        {"MACRO_1"},
        {"MACRO_2"},
        {"MACRO_3"},
        {"MACRO_4"},
        {"MACRO_5"},
        {"MACRO_6 "},

    };
};

#endif