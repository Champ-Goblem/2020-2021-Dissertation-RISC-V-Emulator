#include "include/screen/screen.h"
int main(int argc, char** argv) {
  EmulatorScreen screen(4);
  vector<bytes> r(32);
  for (uint i=0; i<r.size(); i++) {
    r[i] = bytes(4);
  }
  // Element e = screen.renderMemory(r, 0);
  ostringstream oss;
  oss << "A aodijawoidjwaiodjoiwahdwahd" << endl << "multi-line" << "\n\t" << "tabbed" << endl << "\t" << "more stufff" << endl << "\t" << "endl" << endl << "aaaa";
  Element e = screen.renderSTDOut(oss.str());

  Screen out = Screen::Create(Dimension::Full(), Dimension::Fit(e));
  Render(out, e.get());

  cout << out.ToString() << endl;
}

// Memory memory(3000);
// RV32I base = RV32I();
// memory.writeWord(0, bytes{0x93, 0x80, 0xa0, 0x00});
// memory.writeWord(4, bytes{0x13, 0x01, 0x11, 0x00});
// memory.writeWord(8, bytes{0xe3, 0x9e, 0x20, 0xfe});
// memory.writeWord(12, bytes{0x33, 0x01, 0x00, 0x00});
// memory.writeWord(16, bytes{239, 241, 31, 255});
// memory.writeWord(20, bytes{0x33, 0, 0, 0});
// memory.writeWord(24, bytes{0x33, 0, 0, 0});
// memory.writeWord(28, bytes{0x33, 0, 0, 0});
// memory.writeWord(32, bytes{0x33, 0, 0, 0});
// memory.writeWord(36, bytes{0x33, 0, 0, 0});
// memory.writeWord(40, bytes{0x33, 0, 0, 0});
// Hart hart1(&memory, base, ExtensionSet(0), 4, bytes{0, 0, 0, 0}, false);
// Hart hart2(&memory, base, ExtensionSet(0), 4, bytes{0, 0, 0, 0}, false);
// try {
//     for (int i=0; i <= 100; i++) {
//         exception_ptr h1, h2;
//         // hart1.tick();
//         thread hartThread1 = thread(&Hart::tick, &hart1, h1);
//         thread hartThread2 = thread(&Hart::tick, &hart2, h2);
//         hartThread1.join();
//         hartThread2.join();
//     }
// } catch (EmulatorException e) {
//     cerr << e.getMessage();
// } catch (exception e) {
//     cerr << e.what();
// }
// cout << "";