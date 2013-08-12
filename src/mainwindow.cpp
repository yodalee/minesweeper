#include <QtGui>

#include <stdio.h>

#include "mainwindow.h"

MainWindow::MainWindow()
{
	//create the view
	createActions();
	createMenus();
	createSignalMapper();
	createStatusBar();

	setWindowIcon(QIcon(":/images/icon.png"));

	//create the minesweeper
	m_minesweeper = NULL;
}

void 
MainWindow::createActions() 
{
	//File menu action 
	m_newAction = new QAction(tr("&new"), this);
	m_newAction->setShortcut(tr("F2"));
	m_newAction->setStatusTip(tr("Start a new game"));
	connect(m_newAction, SIGNAL(triggered()),
			this, SLOT(newGame()));
	
	m_closeAction = new QAction(tr("&close"), this);
	m_closeAction->setShortcut(tr("Ctrl+Q"));
	m_closeAction->setStatusTip(tr("Exit minesweeper"));
	connect(m_closeAction, SIGNAL(triggered()),
			this, SLOT(closeGame()));

	m_pauseAction = new QAction(tr("&pause"), this);
	m_pauseAction->setShortcut(tr("F3"));
	m_pauseAction->setStatusTip(tr("Pause the game"));
	connect(m_pauseAction, SIGNAL(triggered()),
			this, SLOT(pauseGame()));

	m_recordAction = new QAction(tr("&highscore"), this);
	m_recordAction->setStatusTip(tr("See highscore"));
	connect(m_closeAction, SIGNAL(triggered()),
			this, SLOT(showRecord()));

	//Help menu actions
	m_aboutAction = new QAction(tr("&about"), this);
	m_aboutAction->setShortcut(tr("F1"));
	m_aboutAction->setStatusTip(tr("About the game"));
	connect(m_aboutAction, SIGNAL(triggered()),
			this, SLOT(aboutGame()));
}

void 
MainWindow::closeEvent(QCloseEvent *event)
{
	event->accept();
}

void 
MainWindow::createMenus() 
{
	m_fileMenu = menuBar()->addMenu(tr("&Game"));
	m_fileMenu->addAction(m_newAction);
	m_fileMenu->addAction(m_pauseAction);
	m_fileMenu->addAction(m_recordAction);
	m_separateAction = m_fileMenu->addSeparator();
	m_fileMenu->addAction(m_closeAction);

	m_helpMenu = menuBar()->addMenu(tr("help"));
	m_helpMenu->addAction(m_aboutAction);
}

void 
MainWindow::createStatusBar() 
{
	m_mineLabel = new QLabel("xxxx xxxxxx: www/www");
	m_mineLabel->setAlignment(Qt::AlignHCenter);
	m_mineLabel->setMinimumSize(m_mineLabel->sizeHint());

	m_timeLabel = new QLabel;

	statusBar()->addWidget(m_mineLabel);
	statusBar()->addWidget(m_timeLabel);
};

void 
MainWindow::createSignalMapper() 
{
	m_leftSignalMapper = new QSignalMapper(this);
	m_rightSignalMapper = new QSignalMapper(this);
	connect(m_leftSignalMapper, SIGNAL(mapped(int)), this, SLOT(clickLeftButton(int)));
	connect(m_rightSignalMapper, SIGNAL(mapped(int)), this, SLOT(clickRightButton(int)));
}

void
MainWindow::newGame()
{
	if (m_minesweeper != NULL) { delete m_minesweeper; }
	m_minesweeper = new Minesweeper(g_width, g_height, g_minenum);
	m_land = new QLandButton*[g_width*g_height];
	//create layout
	m_window = new QWidget();
	QHBoxLayout* row = new QHBoxLayout;
	for (int x = 0; x < g_width; ++x) {
		QVBoxLayout* column = new QVBoxLayout;
		for (int y = 0; y < g_height; ++y) {
			//QLandButton* cur = m_land[x*g_height+y];
			m_land[x*g_height+y] = new QLandButton(m_window);
			m_land[x*g_height+y] ->setIcon(QIcon(":/images/null.png"));
			m_land[x*g_height+y] ->setIconSize(QSize(10,10));
			m_leftSignalMapper->setMapping(m_land[x*g_height+y], x*g_height+y);
			m_rightSignalMapper->setMapping(m_land[x*g_height+y], x*g_height+y);
			connect(m_land[x*g_height+y], SIGNAL(clicked()),
					m_leftSignalMapper, SLOT(map()));
			connect(m_land[x*g_height+y], SIGNAL(rightClicked()),
					m_rightSignalMapper, SLOT(map()));
			column->addWidget(m_land[x*g_height+y]);
		}
		row->addLayout(column);
	}
	m_window->setLayout(row);
	setCentralWidget(m_window);
	updateView();
}

void MainWindow::pauseGame(){}
void MainWindow::closeGame(){
	for (int x = 0; x < g_width; ++x) {
		for (int y = 0; y < g_height; ++y) {
			delete m_land[x*g_height+y];
		}
	}
}

void
MainWindow::showRecord()
{
}
void
MainWindow::saveRecord()
{
}

void
MainWindow::updateView()
{
	int i = 0;
	for (iter pos = m_minesweeper->begin(); pos != m_minesweeper->end(); ++pos) {
		switch((*pos)->m_mark){
		  case QUESTION:
			m_land[i]->setIcon(QIcon(":/images/q.png"));
			break;
		  case MARK:
			m_land[i]->setIcon(QIcon(":/images/flag.png"));
			break;
		  default: 
			if (!((*pos)->m_isFound)) {
				m_land[i]->setIcon(QIcon(":/images/null.png"));
			} else {
				int landType = (*pos)->m_land;
				if (landType == MINE) {
					m_land[i]->setIcon(QIcon(tr(":/images/icon.png")));
				} else {
					m_land[i]->setIcon(QIcon(tr(":/images/%1.png").arg(landType)));
				}

			}
			break;
		}
		++i;
	}
}

void
MainWindow::aboutGame()
{
	QMessageBox::about(this, tr("About Minesweeper"),
			tr("<h2>Minesweeper 0.9</h2>"
			   "<p>Copyright &copy; 2013 Yodalee"
			   "<p>Minesweeper is a first view minesweeper game"
			   " wish you like it"));
}

void 
MainWindow::clickLeftButton(int index)
{
	printf("Left Click: %d\n", index);
	statusType status = m_minesweeper->touchLand(index/g_height, index%g_height);
	switch(status){
	  case statusMINE:
		  printf("This is a mine, BOOM!\n");
		  closeGame();
	      break;
	  case statusEND:
		  printf("You WIN =w=b\n");
		  closeGame();
		  break;
	}
	updateView();
}

void 
MainWindow::clickRightButton(int index)
{
	printf("Right Click: %d\n", index);
	m_minesweeper->markLand(index/g_height, index%g_height);
	updateView();
}
