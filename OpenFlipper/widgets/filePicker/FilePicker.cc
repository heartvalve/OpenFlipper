/*
 * FilePicker.cc
 *
 *  Created on: Jan 8, 2013
 *      Author: ebke
 */

#include "FilePicker.hh"

#include <QHBoxLayout>
#include <QMessageBox>

FilePicker::FilePicker(QWidget *parent) :
    QWidget(parent),
    textBox_(new QComboBox),
    browseButton_(new QPushButton(trUtf8("Browse..."))),
    recent_files_ini_key_("GenericFilePickerRecents"),
    dialog_type_(DT_OPEN_FILE),
    overwrite_confirmed_by_user_(false) {

    init();
}

FilePicker::FilePicker(QString recent_files_ini_key,
                           DialogType dialog_type,
                           QWidget *parent)  :
    QWidget(parent),
    textBox_(new QComboBox),
    browseButton_(new QPushButton(trUtf8("Browse..."))),
    recent_files_ini_key_(recent_files_ini_key),
    dialog_type_(dialog_type),
    overwrite_confirmed_by_user_(false) {

    init();
}

void FilePicker::init() {
    /*
     * Do layout stuff.
     */
    textBox_->setSizePolicy(QSizePolicy::MinimumExpanding,
                            textBox_->sizePolicy().verticalPolicy());
    textBox_->setEditable(true);
    textBox_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(textBox_);
    layout->addWidget(browseButton_);

    /*
     * Do functional stuff.
     */
    textBox_->insertItems(
            0,
            OpenFlipper::Options::getRecentItems(recent_files_ini_key()));
    connect(browseButton_, SIGNAL( clicked() ), this, SLOT( slot_browse() ));
    //editTextChanged(const QString &);
    connect(textBox_, SIGNAL( editTextChanged(const QString &) ),
            this, SLOT( path_changed() ));
}

void FilePicker::set_recent_files_ini_key(QString value) {
    recent_files_ini_key_ = value;
    textBox_->clear();
    textBox_->insertItems(
            0,
            OpenFlipper::Options::getRecentItems(recent_files_ini_key_));
}

void FilePicker::path_changed() {
    overwrite_confirmed_by_user_ = false;
}

bool FilePicker::confirm_overwrite_if_necessary() {
    if (overwrite_confirmed_by_user()) return true;
    if (dialog_type() != DT_SAVE_FILE) return true;
    if (!QFile::exists(currentFileName())) return true;

    const QString file_name = QFileInfo(currentFileName()).fileName();
    const QMessageBox::StandardButton result =
            QMessageBox::question(
                    this, trUtf8("Overwrite File?"),
                    trUtf8("The file \"%1\" exists. "
                            "Do you want to overwrite?")
                            .arg(file_name),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

    if (result == QMessageBox::Yes) {
        overwrite_confirmed_by_user_ = true;
        return true;
    } else {
        return false;
    }
}

void FilePicker::slot_browse() {
    OpenFlipper::Options::obtainPathName(
            textBox_,
            dialog_title().toUtf8().constData(),
            file_filter().toUtf8().constData(),
            recent_files_ini_key().toUtf8().constData(),
            static_cast<OpenFlipper::Options::DialogType>(dialog_type()));

    std::cout << "overwrite confirmed." << std::endl;
    overwrite_confirmed_by_user_ = true;
}

FilePicker::~FilePicker() {
}
