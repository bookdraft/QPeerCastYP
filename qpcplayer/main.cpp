#include "qpcplayer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (app.arguments().count() < 2) {
        printf("Usage: %s stream-url [-window-title title] [mplayer-options]\n", argv[0]);
        return 1;
    }

    QString title;
    QStringList mplayerArgs;
    QStringList args = app.arguments();
    for (int i = 1; i < args.count(); ++i) {
        QString arg = args[i];
        if (arg.startsWith("-window-title")) {
            title = args[++i];
        } else {
            mplayerArgs += arg;
        }
    }

    QPCPlayer pcplayer(mplayerArgs);
    pcplayer.setFocusPolicy(Qt::NoFocus);
    pcplayer.setMouseTracking(true);
    pcplayer.setWindowTitle(title.isEmpty() ? "QPCPlayer" : title);
    pcplayer.show();

    return app.exec();
}

