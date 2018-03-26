//
//  main.cpp
//  start-app
//
//  Created by Bartholomew Joyce on 26/03/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

#include <ddui/app>

void update(Context ctx) {

    // The crucial update() function!
    // All the UI work is done in here:
    //    - redrawing all the UI
    //    - responding to user input

}

int main(int argc, const char** argv) {

    app::init("Start App");

    app::load_font_face("regular",        "assets/SFRegular.ttf");
    app::load_font_face("medium",         "assets/SFMedium.ttf");
    app::load_font_face("bold",           "assets/SFBold.ttf");
    app::load_font_face("regular-italic", "assets/SFRegularItalic.ttf");
    app::load_font_face("medium-italic",  "assets/SFMediumItalic.ttf");
    app::load_font_face("bold-italic",    "assets/SFBoldItalic.ttf");

    app::run(&update);

    return 0;
}

