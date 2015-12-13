#ifndef H_MSGBOX
#define H_MSGBOX

enum MsgBoxType {
  NONE,
  INFO,
  WARN,
  ERR,
};

enum MsgBoxBtn {
  OK = 1,
  CANCEL = 2,
};

int msgbox_show(const char* message, const char* title, enum MsgBoxType type, unsigned int buttons);

#endif
