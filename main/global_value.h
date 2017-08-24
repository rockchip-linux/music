#ifndef GLOBAL_VALUE_H
#define GLOBAL_VALUE_H

#include <QStandardPaths>
#include <mainwindow.h>

extern MainWindow *mainWindow;

#ifdef DEVICE_EVB
const int font_size = 32;
const int font_size_big = 40;
const int font_size_large = 60;
// top part
const QString return_resource_str = ":/image/main/return_big.png";
const int return_icon_width = 212;
const int return_icon_height = 70;
const int top_icon_size = 60;
#else
const int font_size = 17;
const int font_size_big = 20;
const int font_size_large = 35;
// top part
const QString return_resource_str = ":/image/main/return.png";
const int return_icon_width = 115;
const int return_icon_height = 40;
const int top_icon_size = 40;
#endif

// music
const QString MUSIC_SEARCH_PATH = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).append("/mnt");
const QString str_add_song= "Add music";
const QString str_clear_list= "Clear list";
const QString str_header_song_list= "歌曲";
const QString str_header_song_type= "类型";
const QString str_song_playing= "Playing";
const QString str_playmode_list= "List";
const QString str_playmode_random= "Random";
const QString str_playmode_single= "Single";
const QString str_origin_move_songname = "Please first select one music";
#endif
