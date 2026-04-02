/**
 * @file texteditor.h
 * @brief Dichiara le classi del mio editor di testo e i loro membri.
 */

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>
#include <QCheckBox>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui {
class TextEditor;
}
QT_END_NAMESPACE

/**
 * @brief Classe che definisce la finestra del editor di testo.
 *
 * Una classe che estende la classe della QMainWindow del framework Qt.
 * Quando viene istanziata crea una finestra del mio editor di testo.
 */
class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

private:
    /**
     * @brief Classe per la azione di menu Modifica>Cerca
     *
     * Viene istanziata una volta sola. Il suo scopo è di raggruppare insieme oggetti
     * relativi alla ricerca e sostituzione nel testo, spostare la loro costruzione fuori
     * dal costruttore di TextEditor e imporre limiti di accesso con information hiding.
     * Questo è stato fatto principalmente per rendere il codice più leggibile.
     */
    class FindAndReplaceDialog {
    public:
        /// finestra di dialogo.
        QDialog dialog;
        /// Componente del UI dove inserire la parola da cercare.
        QLineEdit findEdit;
        /// Componente del UI dove inserire la parola per la sostituzione.
        QLineEdit replaceEdit;
        /// Componente del UI per scegliere se rendere la ricerca case sensitive.
        QCheckBox caseSensitiveCheckBox;
        /// Componente del UI per scegliere se cercare solo parole intere.
        QCheckBox wholeWordsCheckBox;
        /// Componente del UI per eseguire la ricerca.
        QPushButton findButton;
        /// Componente del UI per eseguire la sostituzione.
        QPushButton replaceButton;
        /// Componente del UI per eventuali messaggi di errore.
        QLabel messageLabel;

        FindAndReplaceDialog(TextEditor* window);
    private:
        QLabel findLabel;
        QLabel replaceLabel;
        QVBoxLayout mainLayout;
        QHBoxLayout findLayout;
        QHBoxLayout replaceLayout;
        QHBoxLayout checkBoxLayout;
        QHBoxLayout buttonLayout;
    };

    /// Puntatore all'interfaccia utente progetata con Qtcreator
    Ui::TextEditor *ui;
    /// Il nome del file attualmente in uso. Stringa vuota se il file attuale è senza nome.
    QString currentFile;
    /// Oggetto per la gestione della finestra di dialogo per la ricerca e sostituzione nel testo.
    FindAndReplaceDialog findAndReplace;
    bool promptForSave();
    void closeEvent(QCloseEvent *event);

private slots:
    void createNewFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void findWord();
    void replaceWord();
    void showFindAndReplaceDialog();
};
#endif // TEXTEDITOR_H
