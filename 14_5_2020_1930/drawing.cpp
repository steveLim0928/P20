#include "drawing.h"

drawing::drawing(QWidget *parent)
    : QWidget(parent)
{
    open();
}


// Sender
void drawing::mousePressEvent(QMouseEvent *event) // start drawing
{
    if ((event->button() == Qt::LeftButton) && !receiver) { // left click and not on receive canvas
        lastPoint = event->pos();
        paint = true; //initialise painting

        //serialise initialising painting
        QByteArray buffer;
        QDataStream serial(&buffer, QIODevice::WriteOnly);
        serial << paint;
        emit(mousePress(buffer));

        // serialise last pt
        QByteArray buffer2;
        QDataStream serial2(&buffer2, QIODevice::WriteOnly);
        serial2 << lastPoint;
        emit(lastPt(buffer2));
    }
}
void drawing::mouseMoveEvent(QMouseEvent *event) // drawing
{
    if ((event->buttons() & Qt::LeftButton) && paint && !receiver) // drawing is initialised and not on receive canvas
        draw(event->pos());

    // Serialise current pos
    QByteArray buffer2;
    QDataStream serial2(&buffer2, QIODevice::WriteOnly);
    serial2 << event->pos();
    emit(sendData(buffer2));
}
void drawing::mouseReleaseEvent(QMouseEvent *event) // end drawing
{
    if (event->button() == Qt::LeftButton && paint && !receiver) {
        draw(event->pos());
        paint = false; // stop drawing

        // serialise stop drawing
        QByteArray buffer;
        QDataStream serial(&buffer, QIODevice::WriteOnly);
        serial << paint;
        emit(mousePress(buffer));

        // serialise last pt
        QByteArray buffer2;
        QDataStream serial2(&buffer2, QIODevice::WriteOnly);
        serial2 << event->pos();
        emit(lastPt(buffer2));
    }
}
void drawing::createMenu() { // Top menu bar

    QVBoxLayout *boxLayout = new QVBoxLayout(this); // Main layout of widget
    QMenuBar* menuBar = new QMenuBar(); // top menu bar

    // two main options
    QMenu *brushMenu = new QMenu(tr("&Brush"), this);
    QMenu *canvasMenu = new QMenu(tr("&Canvas"), this);

    menuBar->addMenu(brushMenu);
    menuBar->addMenu(canvasMenu);
    // each smaller child option
    brushMenu->addAction(sizeAction);
    brushMenu->addAction(widthAction);
    canvasMenu->addAction(clearsAction);
    canvasMenu->addAction(canvasAction);

    this->layout()->setMenuBar(menuBar); // display menu bar
}
void drawing::createAction() { // smaller menu bar child option command
    sizeAction = new QAction(tr("&Colour"), this);
    connect(sizeAction, &QAction::triggered, this, &drawing::changePenColour);

    widthAction = new QAction(tr("&Width"), this);
    connect(widthAction, &QAction::triggered, this, &drawing::changePenSize);

    clearsAction = new QAction(tr("&Clear Screen"), this);
    connect(clearsAction, &QAction::triggered, this, &drawing::clear);

    canvasAction = new QAction(tr("&Canvas Size"), this);
    connect(canvasAction, &QAction::triggered, this, &drawing::changeCanvasSize);

}
void drawing::changePenColour() { // change user pen colour

    QColor userColour = QColorDialog::getColor(penColour); // open up a selections of colour
    if (userColour.isValid()) {
        penColour = userColour;

        //serialise
        QByteArray buffer;
        QDataStream serial(&buffer, QIODevice::WriteOnly);
        serial << penColour;
        emit(colourSignal(buffer));
    }
}
void drawing::changePenSize() // change user pen Width
{
    bool check;
    int newSize = QInputDialog::getInt(this, tr("Size"),
                                       tr("Enter pen size:"),
                                       penWidth,
                                       1, 30, 1, &check);
    // within range
    if (check) {
        penWidth = newSize;

        //serialise
        QByteArray buffer;
        QDataStream serial(&buffer, QIODevice::WriteOnly);
        serial << penWidth;
        emit(widthSignal(buffer));
    }
}
void drawing::changeCanvasSize() { // dialog to allow user to enter canvas size

    dialog = new QDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose, false);

    QFormLayout layout(dialog);

    layout.addRow(new QLabel("Enter Your Canvas Size"));

    width = new QLineEdit(dialog);
    QString widthLabel = QString("Width");

    height = new QLineEdit(dialog);
    QString heightLabel = QString("Height");

    layout.addRow(widthLabel, width);
    layout.addRow(heightLabel, height);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);

    layout.addRow(&buttonBox);

    QObject::connect(&buttonBox, SIGNAL(accepted()), this, SLOT(getResizeData()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    dialog->exec();
}
void drawing::getResizeData() { // when user enter new canvas size
    windowSize.setWidth(width->text().toInt());
    windowSize.setHeight(height->text().toInt());
    dialog->close(); // close dialog
    resizeImage(&canvasTemplate, windowSize); // resize canvas

    // serialise
    QByteArray buffer;
    QDataStream serial(&buffer, QIODevice::WriteOnly);
    serial << windowSize;
    emit(canvasSizeSignal(buffer));
}


// Receiver
void drawing::toggleDraw(QByteArray data) { // receive to initialise drawing

    //deserialise
    QDataStream deserial(data);
    bool toggle;
    deserial >> toggle;
    qDebug() << "Received toggle: " << toggle;

    paint = toggle;
}
void drawing::onReceivePos(QByteArray data) { // receive pt to draw

    // deserialise
    QDataStream deserial(data);
    QPoint point;
    deserial >> point;
    qDebug() << point;

    draw(point);
}
void drawing::onReceiveLastPt(QByteArray data) { // receive last point

    // deserialise
    QDataStream deserial(data);
    QPoint point;
    deserial >> point;
    qDebug() << "Received last point: ";

    lastPoint = point;
}
void drawing::onClear() { // receive to clear canvas
    clear();
}
void drawing::onPenSize(QByteArray data) { // receive to change pen size

    // deserialise
    QDataStream deserial(data);
    int point;
    deserial >> point;

    penWidth = point;
}
void drawing::onPenColour(QByteArray data) { // receive to change pen colour

    // deserialise
    QDataStream deserial(data);
    QColor point;
    deserial >> point;

    penColour = point;
}
void drawing::onResizeCanvas(QByteArray data) { // receive to change canvas Size

    //deserialise
    QDataStream deserial(data);
    QSize point;
    deserial >> point;

    resizeImage(&canvasTemplate, point);
}


// Shared
void drawing::resizeImage(QImage *canvasTemplate, const QSize newSize) // resize image
{
    qDebug() << "Resizing";
    if(canvasTemplate->size() == newSize) // if same size, do nothing
        return;

    QImage newcanvasTemplate(newSize, QImage::Format_RGB32); // create new image with new size receive
    newcanvasTemplate.fill(qRgb(255,255,255)); // set new image background to white
    QPainter painter(&newcanvasTemplate);
    painter.drawImage(QPoint(0,0), *canvasTemplate); // paint old image ontop of new from top left
    *canvasTemplate = newcanvasTemplate; // make current image to default
    qDebug() << "Resize done";
    update();
}
void drawing::open(){ // Open initial template to let user draw

    QImage image(QSize(450, 450), QImage::Format_RGB32);
    image.fill(qRgb(255, 255, 255));

    canvasTemplate = image;

    update();
}
void drawing::clear() // clear the canvas
{
    canvasTemplate.fill(qRgb(255, 255, 255));
    update();

    //serialise
    QByteArray buffer;
    QDataStream serial(&buffer, QIODevice::WriteOnly);
    serial << 'c';
    emit(clearSignal(buffer));
}
void drawing::paintEvent(QPaintEvent *event) // paint the whole image
{
    // paint as rect
    QPainter painter(this);
    QRect rect = event->rect();
    painter.drawImage(rect, canvasTemplate, rect);
}
void drawing::type(bool in) { // choose between receiver and sender
    // 0-> sender, 1-> receiver
    receiver = in;
    if (!receiver) { // create all the options for sender only
        createAction();
        createMenu();
    }
}
void drawing::draw(const QPoint &pos) // draw each point given position
{

    QPainter painter(&canvasTemplate);
    painter.setPen(QPen(penColour, penWidth));
    painter.drawLine(lastPoint, pos);

    update();
    lastPoint = pos;
}


//Unused

//void drawing::colourSelector(const QColor &userColour) { // set the pen colour
//    penColour = userColour;

//    //serialise
//    QByteArray buffer;
//    QDataStream serial(&buffer, QIODevice::WriteOnly);
//    serial << penColour;
//    emit(colourSignal(buffer));

//}

//void drawing::widthSelector(const int &width) { // set pen width
//    penWidth = width;
//    QByteArray buffer;
//    QDataStream serial(&buffer, QIODevice::WriteOnly);
//    serial << penWidth;
//    emit(widthSignal(buffer));
//}
