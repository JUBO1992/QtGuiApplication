/*! \file notepad.cpp
	\brief
	\author created by JUBO,bo.ju@southgis.com
	\version v1.0
	\date 2018.05.18
*/

#include <QToolButton>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QTextStream>
#include <QColorDialog>
#include <QTextList>
#include <QTextCodec>//无法实现中文支持
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QPainter>

#include "notepadwindow.h"

NotepadWindow::NotepadWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setWindowTitle(tr("Easy Word"));
	setWindowState(Qt::WindowMaximized);

	_show_widget = new NotepadWidget(this);
	setCentralWidget(_show_widget);

	_font_label1 = new QLabel(tr("Font:"));
	_font_combobox = new QFontComboBox;
	_font_combobox->setFontFilters(QFontComboBox::ScalableFonts);

	_font_label2 = new QLabel(tr("Size:"));
	_size_combobox = new QComboBox;
	QFontDatabase db;
	foreach(int size, db.standardSizes())
	{
		_size_combobox->addItem(QString::number(size));
	}

	_bold_btn = new QToolButton;
	_bold_btn->setIcon(QIcon("png/bold.png"));
	_bold_btn->setCheckable(true);
	_italic_btn = new QToolButton;
	_italic_btn->setIcon(QIcon("png/italic.png"));
	_italic_btn->setChecked(true);
	_underline_btn = new QToolButton;
	_underline_btn->setIcon(QIcon("png/underline.png"));
	_underline_btn->setCheckable(true);
	_color_btn = new QToolButton;
	_color_btn->setIcon(QIcon("png/color.png"));
	_color_btn->setCheckable(true);

	_list_label = new QLabel(tr("Sort:"));
	_list_combobox = new QComboBox;
	_list_combobox->addItem("STandard");
	_list_combobox->addItem("QTextListFormat::ListDisc");
	_list_combobox->addItem("QTextListFormat::ListCircle");
	_list_combobox->addItem("QTextListFormat::ListSquare");
	_list_combobox->addItem("QTextListForamt::ListDecimal");
	_list_combobox->addItem("QTextListFormat::ListLowerAlpha");
	_list_combobox->addItem("QTextListFormat::ListUpperAlpha");
	_list_combobox->addItem("QTextListFormat::ListLowerRoman");
	_list_combobox->addItem("QTextListFormat::ListUpperRoman");

	createActions();
	createMenus();
	createToolBars();

	if (_img.load("image.png"))
	{
		_show_widget->_img_label->setPixmap(QPixmap::fromImage(_img));
	}

	connect(_font_combobox, SIGNAL(activated(QString)), this, SLOT(showFontComboBox(QString)));
	connect(_size_combobox, SIGNAL(activated(QString)), this, SLOT(showSizeSpinBox(QString)));
	connect(_bold_btn, SIGNAL(clicked()), this, SLOT(showBoldBtn()));
	connect(_italic_btn, SIGNAL(clicked()), this, SLOT(showItalicBtn()));
	connect(_underline_btn, SIGNAL(clicked()), this, SLOT(showUnderlineBtn()));
	connect(_color_btn, SIGNAL(clicked()), this, SLOT(showColorBtn()));
	connect(_show_widget->_text, SIGNAL(currentCharFormatChanged(QTextCharFormat&)), this, 
		SLOT(showCurrentFormatChanged(QTextCharFormat&)));
	
	connect(_list_combobox, SIGNAL(activated(int)), this, SLOT(showList(int)));
	connect(_show_widget->_text->document(), SIGNAL(undoAvailable(bool)), _undo_action, SLOT(setEnabled(bool)));
	connect(_show_widget->_text->document(), SIGNAL(redoAvailable(bool)), _redo_action, SLOT(setEnabled(bool)));
	connect(_show_widget->_text, SIGNAL(cursorPositionChanged()), this, SLOT(showCurrentFormatChanged()));
}

NotepadWindow::~NotepadWindow()
{

}

void NotepadWindow::createActions()
{
	//打开
	_open_file_action = new QAction(QIcon("png/open.png"), tr("Open"), this);
	_open_file_action->setShortcut(tr("Ctrl+O"));
	_open_file_action->setStatusTip(tr("Open a File"));
	connect(_open_file_action, SIGNAL(triggered()), this, SLOT(showOpenFile()));
	
	//新建
	_new_file_action = new QAction(QIcon("png/new.png"), tr("New"), this);
	_new_file_action->setShortcut(tr("Ctrl+N"));
	_new_file_action->setStatusTip(tr("New a File"));
	connect(_new_file_action, SIGNAL(triggered()), this, SLOT(showNewFile()));

	//退出
	_exit_action = new QAction(tr("Exit"), this);
	_exit_action->setShortcut(tr("Ctrl+Q"));
	_exit_action->setStatusTip(tr("Exit Application"));
	connect(_exit_action, SIGNAL(triggered()), this, SLOT(close()));

	//复制
	_copy_action = new QAction(QIcon("png/copy.png"), tr("Copy"), this);
	_copy_action->setShortcut(tr("Ctrl+C"));
	_copy_action->setStatusTip(tr("Copy File"));
	connect(_copy_action, SIGNAL(triggered()), _show_widget->_text, SLOT(copy()));

	//"剪切"动作
	_cut_action = new QAction(QIcon("png/cut.png"), tr("Cut"), this);
	_cut_action->setShortcut(tr("Ctrl+X"));
	_cut_action->setStatusTip(tr("Cut Texts"));
	connect(_cut_action, SIGNAL(triggered()), _show_widget->_text, SLOT(cut()));

	//"粘贴"动作
	_paste_action = new QAction(QIcon("png/paste.png"), tr("Paste"), this);
	_paste_action->setShortcut(tr("Ctrl+V"));
	_paste_action->setStatusTip(tr("Paste Texts"));
	connect(_paste_action, SIGNAL(triggered()), _show_widget->_text, SLOT(paste()));

	//"关于"动作
	_about_action = new QAction(tr("About"), this);
	connect(_about_action, SIGNAL(triggered()), this, SLOT(QApplication::aboutQt()));

	//"打印文本"动作
	_print_text_action = new QAction(QIcon("png/printText.png"), tr("Print Text"), this);
	_print_text_action->setStatusTip(tr("Print Text"));
	connect(_print_text_action, SIGNAL(triggered()), this, SLOT(showPrintTetxt()));

	//"打印图像"动作
	_print_image_action = new QAction(QIcon("png/printImage.png"), tr("Print Img"), this);
	_print_image_action->setStatusTip(tr("Print Image"));
	connect(_print_image_action, SIGNAL(triggered()), this, SLOT(showPrintImage()));

	//"放大"动作
	_zoomin_action = new QAction(QIcon("png/zoomin.png"), tr("Zoom In"), this);
	_zoomin_action->setStatusTip(tr("Zoomin a Image"));
	connect(_zoomin_action, SIGNAL(triggered()), this, SLOT(showZoomIn()));

	//"缩小"动作
	_zoomout_action = new QAction(QIcon("png/zoomout.png"), tr("Zoom Out"), this);
	_zoomout_action->setStatusTip(tr("Zoomout a Image"));
	connect(_zoomout_action, SIGNAL(triggered()), this, SLOT(showZoomOut()));

	//实现图像旋转的动作（Action）
	//旋转90°
	_rotate90_action = new QAction(QIcon("png/rotate90.png"), tr("Rotate90"), this);
	_rotate90_action->setStatusTip(tr("Rotate90°"));
	connect(_rotate90_action, SIGNAL(triggered()), this, SLOT(showRotate90()));

	//旋转180°
	_rotate180_action = new QAction(QIcon("png/rotate180.png"), tr("Rotate180"), this);
	_rotate180_action->setStatusTip(tr("Rotate180°"));
	connect(_rotate180_action, SIGNAL(triggered()), this, SLOT(showRotate180()));

	//旋转270°
	_rotate270_action = new QAction(QIcon("png/rotate270.png"), tr("Rotate270"), this);
	_rotate270_action->setStatusTip(tr("Rotate270°"));
	connect(_rotate270_action, SIGNAL(triggered()), this, SLOT(showRotate270()));

	//实现图像镜像的动作（Action）
	//纵向镜像
	_mirror_vertival_action = new QAction(tr("Vertical Mirror"), this);
	_mirror_vertival_action->setStatusTip(tr("Vertical Mirror"));
	connect(_mirror_vertival_action, SIGNAL(triggered()), this, SLOT(showMirrorVertical()));

	//横向镜像
	_mirror_horizontal_action = new QAction(tr("Horizontal Mirror"), this);
	_mirror_horizontal_action->setStatusTip(tr("Horizontal Mirror"));
	connect(_mirror_horizontal_action, SIGNAL(triggered()), this, SLOT(showMirrorHorizontal()));

	//排序:左对齐、右对齐、居中和两端对齐
	_act_grp = new QActionGroup(this);
	_left_action = new QAction(QIcon("png/left.png"), tr("Left Justify"), _act_grp);
	_left_action->setCheckable(true);
	_right_action = new QAction(QIcon("png/right.png"), tr("Right Justify"), _act_grp);
	_right_action->setCheckable(true);
	_center_action = new QAction(QIcon("png/center.png"), tr("Center Justify"), _act_grp);
	_center_action->setCheckable(true);
	_justify_action = new QAction(QIcon("png/justify.png"), tr("Justify Justify"), _act_grp);
	_justify_action->setCheckable(true);
	connect(_act_grp, SIGNAL(triggered(QAction*)), this, SLOT(showAlignment(QAction*)));

	//撤销和重做
	_undo_action = new QAction(QIcon("png/undo.png"), tr("Undo"), this);
	connect(_undo_action, SIGNAL(triggered()), _show_widget->_text, SLOT(undo()));
	_redo_action = new QAction(QIcon("png/redo.png"), tr("Redo"), this);
	connect(_redo_action, SIGNAL(triggered()), _show_widget->_text, SLOT(redo()));
}

void NotepadWindow::createMenus()
{
	//文件菜单
	_file_menu = menuBar()->addMenu(tr("File"));
	_file_menu->addAction(_open_file_action);
	_file_menu->addAction(_new_file_action);
	_file_menu->addAction(_print_text_action);
	_file_menu->addAction(_print_image_action);
	_file_menu->addSeparator();
	_file_menu->addAction(_exit_action);

	//缩放菜单
	_zoom_menu = menuBar()->addMenu(tr("Edit"));
	_zoom_menu->addAction(_copy_action);
	_zoom_menu->addAction(_cut_action);
	_zoom_menu->addAction(_paste_action);
	_zoom_menu->addAction(_about_action);
	_zoom_menu->addSeparator();
	_zoom_menu->addAction(_zoomin_action);
	_zoom_menu->addAction(_zoomout_action);

	//旋转菜单
	_rotate_menu = menuBar()->addMenu(tr("Rotate"));
	_rotate_menu->addAction(_rotate90_action);
	_rotate_menu->addAction(_rotate180_action);
	_rotate_menu->addAction(_rotate270_action);

	//镜像菜单
	_mirror_menu = menuBar()->addMenu(tr("Mirror"));
	_mirror_menu->addAction(_mirror_vertival_action);
	_mirror_menu->addAction(_mirror_horizontal_action);
}

void NotepadWindow::createToolBars()
{
	//文件工具条
	_file_tool = addToolBar("File");
	_file_tool->addAction(_open_file_action);
	_file_tool->addAction(_new_file_action);
	_file_tool->addAction(_print_text_action);
	_file_tool->addAction(_print_image_action);

	//编辑工具条
	_zoom_tool = addToolBar("Edit");
	_zoom_tool->addAction(_copy_action);
	_zoom_tool->addAction(_cut_action);
	_zoom_tool->addAction(_paste_action);
	_zoom_tool->addSeparator();
	_zoom_tool->addAction(_zoomin_action);
	_zoom_tool->addAction(_zoomout_action);

	//旋转工具条
	_rotate_tool = addToolBar("Rotate");
	_rotate_tool->addAction(_rotate90_action);
	_rotate_tool->addAction(_rotate180_action);
	_rotate_tool->addAction(_rotate270_action);

	//撤销和重做工具条
	_do_toolbar = addToolBar("DoEdit");
	_do_toolbar->addAction(_undo_action);
	_do_toolbar->addAction(_redo_action);

	//字体工具条
	_font_toolbar = addToolBar("Font");
	_font_toolbar->addWidget(_font_label1);
	_font_toolbar->addWidget(_font_combobox);
	_font_toolbar->addWidget(_font_label2);
	_font_toolbar->addWidget(_size_combobox);
	_font_toolbar->addSeparator();
	_font_toolbar->addWidget(_bold_btn);
	_font_toolbar->addWidget(_italic_btn);
	_font_toolbar->addWidget(_underline_btn);
	_font_toolbar->addSeparator();
	_font_toolbar->addWidget(_color_btn);

	//排序工具条
	_list_toolbar = addToolBar("list");
	_list_toolbar->addWidget(_list_label);
	_list_toolbar->addWidget(_list_combobox);
	_list_toolbar->addSeparator();
	_list_toolbar->addActions(_act_grp->actions());
}

void NotepadWindow::loadFile(QString filename)
{
	printf("file name:%s\n", _filename.data());
	QFile file(_filename);
	if (file.open(QIODevice::ReadOnly|QIODevice::Text))
	{
		QTextStream text_stream(&file);
		while (!text_stream.atEnd())
		{
			_show_widget->_text->append(text_stream.readLine());
			printf("read line\n");
		}
		printf("end\n");
	}
}

void NotepadWindow::mergeFormat(QTextCharFormat format)
{
	QTextCursor cursor = _show_widget->_text->textCursor();
	if (!cursor.hasSelection())
	{
		cursor.select(QTextCursor::WordUnderCursor);
	}
	cursor.mergeCharFormat(format);
	_show_widget->_text->mergeCurrentCharFormat(format);
}

void NotepadWindow::showNewFile()
{
	NotepadWindow *newnp = new NotepadWindow;
	newnp->show();
}

void NotepadWindow::showOpenFile()
{
	_filename = QFileDialog::getOpenFileName(this, "Open");
	if (!_filename.isEmpty())
	{
		if (_show_widget->_text->document()->isEmpty())
		{
			loadFile(_filename);
		}
		else
		{
			NotepadWindow *newnp = new NotepadWindow;
			newnp->show();
			newnp->loadFile(_filename);
		}
	}
}

void NotepadWindow::showPrintTetxt()
{
	QPrinter printer;
	QPrintDialog printDialog(&printer, this);
	if (printDialog.exec())
	{
		QTextDocument *doc = _show_widget->_text->document();
		doc->print(&printer);
	}
}

void NotepadWindow::showPrintImage()
{
	QPrinter printer;
	QPrintDialog printDialog(&printer, this);
	if (printDialog.exec())
	{
		QPainter painter(&printer);
		QRect rect = painter.viewport();
		QSize size = _img.size();
		size.scale(rect.size(), Qt::KeepAspectRatio);

		painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
		painter.setWindow(_img.rect());
		painter.drawImage(0, 0, _img);
	}
}

void NotepadWindow::showZoomIn()
{
	if (_img.isNull())
	{
		return;
	}
	QMatrix matrix;
	matrix.scale(2, 2);
	_img = _img.transformed(matrix);
	_show_widget->_img_label->setPixmap(QPixmap::fromImage(_img));
}

void NotepadWindow::showZoomOut()
{
	if (_img.isNull())
	{
		return;
	}
	QMatrix matrix;
	matrix.scale(0.5, 0.5);
	_img = _img.transformed(matrix);
	_show_widget->_img_label->setPixmap(QPixmap::fromImage(_img));
}

void NotepadWindow::showRotate90()
{
	if (_img.isNull())
	{
		return;
	}
	QMatrix matrix;
	matrix.rotate(90);
	_img = _img.transformed(matrix);
	_show_widget->_img_label->setPixmap(QPixmap::fromImage(_img));
}

void NotepadWindow::showRotate180()
{
	if (_img.isNull())
	{
		return;
	}
	QMatrix matrix;
	matrix.rotate(180);
	_img = _img.transformed(matrix);
	_show_widget->_img_label->setPixmap(QPixmap::fromImage(_img));
}

void NotepadWindow::showRotate270()
{
	if (_img.isNull())
	{
		return;
	}
	QMatrix matrix;
	matrix.rotate(270);
	_img = _img.transformed(matrix);
	_show_widget->_img_label->setPixmap(QPixmap::fromImage(_img));
}

void NotepadWindow::showMirrorVertical()
{
	if (_img.isNull())
	{
		return;
	}
	_img = _img.mirrored(false, true);
	_show_widget->_img_label->setPixmap(QPixmap::fromImage(_img));
}

void NotepadWindow::showMirrorHorizontal()
{
	if (_img.isNull())
	{
		return;
	}
	_img = _img.mirrored(true, false);
	_show_widget->_img_label->setPixmap(QPixmap::fromImage(_img));
}

void NotepadWindow::showFontComboBox(QString comboStr)
{
	QTextCharFormat fmt;
	fmt.setFontFamily(comboStr);
	mergeFormat(fmt);
}

void NotepadWindow::showSizeSpinBox(QString spinValue)
{
	QTextCharFormat fmt;
	fmt.setFontPointSize(spinValue.toFloat());
	_show_widget->_text->mergeCurrentCharFormat(fmt);
}

void NotepadWindow::showBoldBtn()
{
	QTextCharFormat fmt;
	fmt.setFontWeight(_bold_btn->isChecked() ? QFont::Bold : QFont::Normal);
	_show_widget->_text->mergeCurrentCharFormat(fmt);
}

void NotepadWindow::showItalicBtn()
{
	QTextCharFormat fmt;
	fmt.setFontItalic(_italic_btn->isChecked());
	_show_widget->_text->mergeCurrentCharFormat(fmt);
}

void NotepadWindow::showUnderlineBtn()
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(_underline_btn->isChecked());
	_show_widget->_text->mergeCurrentCharFormat(fmt);
}

void NotepadWindow::showColorBtn()
{
	QColor color = QColorDialog::getColor(Qt::red, this);
	if (color.isValid())
	{
		QTextCharFormat fmt;
		fmt.setForeground(color);
		_show_widget->_text->mergeCurrentCharFormat(fmt);
	}
}

void NotepadWindow::showCurrentFormatChanged(const QTextCharFormat &fmt)
{
	_font_combobox->setCurrentIndex(_font_combobox->findText(fmt.fontFamily()));
	_size_combobox->setCurrentIndex(_size_combobox->findText(QString::number(fmt.fontPointSize())));
	_bold_btn->setChecked(fmt.font().bold());
	_italic_btn->setCheckable(fmt.fontItalic());
	_underline_btn->setCheckable(fmt.fontUnderline());
}

void NotepadWindow::showList(int index)
{
	QTextCursor cursor = _show_widget->_text->textCursor();
	if (index != 0)
	{
		QTextListFormat::Style style = QTextListFormat::ListDisc;
		switch (index)
		{
		case 1:
			style = QTextListFormat::ListDisc;
			break;
		case 2:
			style = QTextListFormat::ListCircle;
			break;
		case 3:
			style = QTextListFormat::ListSquare;
			break;
		case 4:
			style = QTextListFormat::ListDecimal;
			break;
		case 5:
			style = QTextListFormat::ListLowerAlpha;
			break;
		case 6:
			style = QTextListFormat::ListUpperAlpha;
			break;
		case 7:
			style = QTextListFormat::ListLowerRoman;
			break;
		case 8:
			style = QTextListFormat::ListUpperRoman;
			break;
		default:
			break;
		}
		cursor.beginEditBlock();

		QTextBlockFormat blockFmt = cursor.blockFormat();
		QTextListFormat listFmt;

		if (cursor.currentList())
		{
			listFmt = cursor.currentList()->format();
		}
		else
		{
			listFmt.setIndent(blockFmt.indent() + 1);
			blockFmt.setIndent(0);
			cursor.setBlockFormat(blockFmt);
		}
		listFmt.setStyle(style);
		cursor.createList(listFmt);

		cursor.endEditBlock();
	}
	else
	{
		QTextBlockFormat bfmt;
		bfmt.setObjectIndex(-1);
		cursor.mergeBlockFormat(bfmt);
	}
}

void NotepadWindow::showAlignment(QAction *act)
{
	if (act == _left_action)
		_show_widget->_text->setAlignment(Qt::AlignLeft);
	if (act == _right_action)
		_show_widget->_text->setAlignment(Qt::AlignRight);
	if (act == _center_action)
		_show_widget->_text->setAlignment(Qt::AlignCenter);
	if (act == _justify_action)
		_show_widget->_text->setAlignment(Qt::AlignJustify);
}

void NotepadWindow::showCursorPositionChanged()
{
	if (_show_widget->_text->alignment() == Qt::AlignLeft)
		_left_action->setChecked(true);
	if (_show_widget->_text->alignment() == Qt::AlignRight)
		_right_action->setChecked(true);
	if (_show_widget->_text->alignment() == Qt::AlignCenter)
		_center_action->setChecked(true);
	if (_show_widget->_text->alignment() == Qt::AlignJustify)
		_justify_action->setChecked(true);
}
