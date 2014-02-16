//#include "gtk_gui.h"
#include "cli.h"
#include "gtk_gui.h"
#include <vector>
#include <string>

using namespace std;

typedef GtkGui::ApplicationRootWindow UIClass;


int main(int argc, char** argv) {

  UIClass ui;

  return ui.main(argc, argv);

}
