/*! \file notepad.h
	\brief 
	\author created by JUBO,bo.ju@southgis.com
	\version v1.0
	\date 2018.05.18
*/

#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include <QImage>
#include <QFontComboBox>
#include "ui_notepad.h"
#include "notepadwidget.h"

class NotepadWindow : public QMainWindow
{
	Q_OBJECT

public:
	NotepadWindow(QWidget *parent = 0);
	~NotepadWindow();

	void createActions();
	void createMenus();
	void createToolBars();
	void loadFile(QString filename);
	void mergeFormat(QTextCharFormat);

protected slots:
	void showNewFile();
	void showOpenFile();
	void showPrintTetxt();
	void showPrintImage();
	void showZoomIn();
	void showZoomOut();
	void showRotate90();
	void showRotate180();
	void showRotate270();
	void showMirrorVertical();
	void showMirrorHorizontal();

	void showFontComboBox(QString comboStr);
	void showSizeSpinBox(QString spinValue);
	void showBoldBtn();
	void showItalicBtn();
	void showUnderlineBtn();
	void showColorBtn();
	void showCurrentFormatChanged(const QTextCharFormat &fmt);

	void showList(int);
	void showAlignment(QAction *act);
	void showCursorPositionChanged();

private:
	Ui::Notepad ui;

	QMenu *_file_menu;
	QMenu *_zoom_menu;
	QMenu *_rotate_menu;
	QMenu *_mirror_menu;

	QImage _img;
	QString _filename;
	NotepadWidget *_show_widget;

	QAction *_open_file_action;
	QAction *_new_file_action;
	QAction *_print_text_action;
	QAction *_print_image_action;
	QAction *_exit_action;

	QAction *_copy_action;
	QAction *_cut_action;
	QAction *_paste_action;
	QAction *_about_action;
	QAction *_zoomin_action;
	QAction *_zoomout_action;

	QAction *_rotate90_action;
	QAction *_rotate180_action;
	QAction *_rotate270_action;

	QAction *_mirror_vertival_action;
	QAction *_mirror_horizontal_action;

	QAction *_undo_action;
	QAction *_redo_action;

	QToolBar *_file_tool;
	QToolBar *_zoom_tool;
	QToolBar *_rotate_tool;
	QToolBar *_mirror_tool;

	QToolBar *_do_toolbar;

	QLabel *_font_label1;
	QFontComboBox *_font_combobox;
	QLabel *_font_label2;
	QComboBox *_size_combobox;
	QToolButton *_bold_btn;
	QToolButton *_italic_btn;
	QToolButton *_underline_btn;
	QToolButton *_color_btn;

	QToolBar *_font_toolbar;

	QLabel *_list_label;
	QComboBox *_list_combobox;
	QActionGroup *_act_grp;
	QAction *_left_action;
	QAction *_right_action;
	QAction *_center_action;
	QAction *_justify_action;

	QToolBar *_list_toolbar;
};

#endif // NOTEPAD_H
