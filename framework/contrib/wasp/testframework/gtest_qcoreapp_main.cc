#include <QCoreApplication>
#include <QThread>
#include <stdio.h>
#include "gtest/gtest.h"
#if defined _WIN32 || defined __CYGWIN__
  #define TFW_PUBLIC __declspec(dllexport)
  #define TFW_LOCAL
#else
  #define TFW_PUBLIC
  #define TFW_LOCAL __declspec(dllimport)
#endif

class TFW_PUBLIC Thread : public QThread
{
private:

    int mArgc;
    char ** mArgv;
public:
    Thread(int& argc, char ** argv)
    {
        mArgc = argc;
        mArgv = argv;
        result = 0;
    }
    int result;

    void run() {
        printf("Running main() from testframework/gtest_qtcoreapp_main.cc\n");
        testing::InitGoogleTest(&mArgc, mArgv);
        result = RUN_ALL_TESTS();
    }
}; // class Thread

int TFW_PUBLIC main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  int result = 0;
  Thread thread(argc, argv);
  thread.start();
  app.connect(&thread
          , SIGNAL(finished())
          , SLOT(quit()));
  result = app.exec();
  return result + thread.result;
}
