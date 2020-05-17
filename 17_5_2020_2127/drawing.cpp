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
        serial << lastPoint;
        emit(sendData(buffer, 't'));
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
    emit(sendData(buffer2, 'p'));
}
void drawing::mouseReleaseEvent(QMouseEvent *event) // end drawing
{
    if (event->button() == Qt::LeftButton && paint && !receiver) {
        draw(event->pos());
        paint = false; // stop drawing

        // serialise stop drawing
        QByteArray buffer;
        QDataStream serial(&buffer, QIODevice::WriteOnly);
        serial << lastPoint;
        emit(sendData(buffer, 't'));

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
    brushMenu->addAction(penAction);
    brushMenu->addAction(penColourAction);
    brushMenu->addSeparator();
    brushMenu->addAction(eraserAction);
    brushMenu->addSeparator();
    brushMenu->addAction(widthAction);
    canvasMenu->addAction(clearsAction);
    canvasMenu->addAction(canvasAction);
    canvasMenu->addAction(canvasColourAction);

    this->layout()->setMenuBar(menuBar); // display menu bar
}
void drawing::createAction() { // smaller menu bar child option command
    penColourAction = new QAction(tr("&Pen Colour"), this);
    connect(penColourAction, &QAction::triggered, this, &drawing::changePenColour);

    widthAction = new QAction(tr("&Eraser/Pen Size"), this);
    connect(widthAction, &QAction::triggered, this, &drawing::changePenSize);

    clearsAction = new QAction(tr("&Clear"), this);
    connect(clearsAction, &QAction::triggered, this, &drawing::clear);

    canvasAction = new QAction(tr("&Resize"), this);
    connect(canvasAction, &QAction::triggered, this, &drawing::changeCanvasSize);

    canvasColourAction = new QAction(tr("&Background Colour"), this);
    connect(canvasColourAction, &QAction::triggered, this, &drawing::changeCanvasColour);

    eraserAction = new QAction(tr("&Eraser"), this);
    connect(eraserAction, &QAction::triggered, this, &drawing::eraser);
    eraserAction -> setCheckable (true);
    eraserAction -> setChecked (0);

    penAction = new QAction(tr("&Pen"), this);
    connect(penAction, &QAction::triggered, this, &drawing::pen);
    penAction -> setCheckable (true);
    penAction -> setChecked (1);

}
void drawing::changePenColour() { // change user pen colour

    QColor userColour = QColorDialog::getColor(penColour); // open up a selections of colour
    if (userColour.isValid()) {
        penNewColour = userColour;
        penColour = userColour;

        //serialise
        QByteArray buffer;
        QDataStream serial(&buffer, QIODevice::WriteOnly);
        serial << penNewColour;
        emit(sendData(buffer, 'c'));
    }
}
void drawing::changePenSize() // change user pen Width
{
    bool check;
    int newSize = QInputDialog::getInt(this, tr("Size"),
                                       tr("Enter pen size:"),
                                       penWidth,
                                       1, 20, 1, &check);
    // within range
    if (check) {
        penWidth = newSize;

        //serialise
        QByteArray buffer;
        QDataStream serial(&buffer, QIODevice::WriteOnly);
        serial << penWidth;
        emit(sendData(buffer, 'w'));
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
    emit(sendData(buffer, 'r'));
}
void drawing::changeCanvasColour() { // when user change canvas coloue

    QColor userColour = QColorDialog::getColor(canvasColour); // open up a selections of colour
    if (userColour.isValid()) {
        canvasNewColour = userColour;

        //serialise
        QByteArray buffer;
        QDataStream serial(&buffer, QIODevice::WriteOnly);
        serial << canvasNewColour;
        emit(sendData(buffer, 'q'));
    }
    colourCanvas();
}
void drawing::eraser() { // when user select eraser

    penColour = canvasColour; // make current pen colour the background colour
    penAction -> setChecked (0); // pen untick under menu
    eraserAction -> setChecked (1); // eraser tick under menu

    //serialise
    QByteArray buffer;
    QDataStream serial(&buffer, QIODevice::WriteOnly);
    serial << penColour;
    emit(sendData(buffer, 'e'));
}
void drawing::pen() { // when user select pen

    penColour = penNewColour; // set current pen to last pen colour
    penAction -> setChecked (1); // pen tick under menu
    eraserAction -> setChecked (0); // eraser untick under menu

    //serialise
    QByteArray buffer;
    QDataStream serial(&buffer, QIODevice::WriteOnly);
    serial << penColour;
    emit(sendData(buffer, 'P'));
}


// Receiver
void drawing::toggleDraw(QByteArray data) { // receive to initialise drawing

    //deserialise
    QDataStream deserial(data);
    QPoint point;
    deserial >> point;
    //qDebug() << "Received toggle: " << toggle;

    paint = !paint;

    lastPoint = point; // draw last point
    draw(point);
}
void drawing::onReceivePos(QByteArray data) { // receive pt to draw

    // deserialise
    QDataStream deserial(data);
    QPoint point;
    deserial >> point;
    //qDebug() << point;

    draw(point);
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
    penNewColour = point;
}
void drawing::onResizeCanvas(QByteArray data) { // receive to change canvas Size

    //deserialise
    QDataStream deserial(data);
    QSize point;
    deserial >> point;

    resizeImage(&canvasTemplate, point);
}
void drawing::onCanvasColour(QByteArray data) { // receive to change canvas colour

    // deserialise
    QDataStream deserial(data);
    QColor point;
    deserial >> point;

    canvasNewColour = point; // set new background colour to data

    colourCanvas();
}
void drawing::onEraser(QByteArray data) { // receive when user select eraser

    // deserialise
    QDataStream deserial(data);
    QColor point;
    deserial >> point;

    penColour = point; // set current pen colour to background colour
}
void drawing::onPen(QByteArray data) { // receive when user select pen

    // deserialise
    QDataStream deserial(data);
    QColor point;
    deserial >> point;

    penColour = point; // set current pen colour to last colour from user side
}


// Shared
void drawing::resizeImage(QImage *canvasTemplate, const QSize newSize) // resize image
{
    qDebug() << "Resizing";
    if(canvasTemplate->size() == newSize) // if same size, do nothing
        return;

    windowSize = newSize;
    QImage newcanvasTemplate(newSize, QImage::Format_RGB32); // create new image with new size receive
    newcanvasTemplate.fill(canvasColour); // set new image background to white
    QPainter painter(&newcanvasTemplate);
    painter.drawImage(QPoint(0,0), *canvasTemplate); // paint old image ontop of new from top left
    *canvasTemplate = newcanvasTemplate; // make current image to default
    qDebug() << "Resize done";
    update();
}
void drawing::open(){ // Open initial template to let user draw

    QImage image(windowSize, QImage::Format_RGB32);
    image.fill(canvasColour);

    canvasTemplate = image;

    update();
}
void drawing::clear() // clear the canvas
{
    canvasTemplate.fill(canvasColour);
    update();

    emit(sendData(0,'C'));
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
    //painter.drawEllipse(lastPoint, penWidth/2, penWidth/2);

    update();
    lastPoint = pos;
}
void drawing::colourCanvas() { // change the background colour of canvas

    // for each pixel, if current pixel colour is same as background, change the colour to new background colour
    for(int y = 0; y < windowSize.rheight(); y++) {
        for(int x = 0; x < windowSize.rwidth(); x++) {
            if (canvasTemplate.pixelColor(x, y) == canvasColour)
                canvasTemplate.setPixelColor(x,y,canvasNewColour);

        }
    }
    // if eraser was selected
    if (penColour == canvasColour) {
        penColour = canvasNewColour;
    }
    // update the new background colour to current
    canvasColour = canvasNewColour;

    update();
}
