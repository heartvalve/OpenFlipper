/*
 * FilePicker.hh
 *
 *  Created on: Jan 8, 2013
 *      Author: ebke
 */

#ifndef FILEPICKER_HH_
#define FILEPICKER_HH_

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <OpenFlipper/common/RecentFiles.hh>

class DLLEXPORT FilePicker : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString recent_files_ini_key
               READ recent_files_ini_key
               WRITE set_recent_files_ini_key)
    Q_PROPERTY(QString dialog_title
               READ dialog_title
               WRITE set_dialog_title)
    Q_PROPERTY(QString file_filter
               READ file_filter
               WRITE set_file_filter)
    Q_PROPERTY(DialogType dialog_type
               READ dialog_type
               WRITE set_dialog_type)
    Q_PROPERTY(QString current_file_name
               READ current_file_name
               WRITE set_current_file_name)
    Q_ENUMS(DialogType)

    public:
        enum DialogType {
            DT_OPEN_FILE = OpenFlipper::Options::DT_OPEN,
            DT_SAVE_FILE = OpenFlipper::Options::DT_SAVE,
            DT_CHOOSE_PATH = OpenFlipper::Options::DT_CHOOSE_PATH
        };

        explicit FilePicker(QWidget *parent = 0);
        explicit FilePicker(QString recent_files_ini_key,
                              DialogType dialog_type,
                              QWidget *parent = 0);
        virtual ~FilePicker();

        QString current_file_name() {
            return textBox_->currentText();
        }

        void set_current_file_name(QString value) {
            return textBox_->lineEdit()->setText(value);
        }

        // Legacy:
        QString currentFileName() {
            return current_file_name();
        }

        QString recent_files_ini_key() { return recent_files_ini_key_; }
        void set_recent_files_ini_key(QString value);

        QString dialog_title() { return dialog_title_; }
        void set_dialog_title(QString value) {
            dialog_title_ = value;
        }

        QString file_filter() { return file_filter_; }
        void set_file_filter(QString value) {
            file_filter_ = value;
        }

        DialogType dialog_type() { return dialog_type_; }
        void set_dialog_type(DialogType value) {
            dialog_type_ = value;
        }

        bool overwrite_confirmed_by_user() {
            return overwrite_confirmed_by_user_;
        }

    public slots:
        bool confirm_overwrite_if_necessary();

    protected slots:
        void slot_browse();
        void path_changed();

    private:
        void init();

    protected:
        QComboBox *textBox_;
        QPushButton *browseButton_;
        QString recent_files_ini_key_;
        QString dialog_title_;
        QString file_filter_;
        DialogType dialog_type_;
        bool overwrite_confirmed_by_user_;
};

#endif /* FILEPICKER_HH_ */
