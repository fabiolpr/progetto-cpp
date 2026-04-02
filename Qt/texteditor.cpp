/**
 * @file texteditor.cpp
 * @brief Definisce le classi del mio editor di testo e i loro membri.
 */

#include "texteditor.h"
#include "ui_texteditor.h"

/**
 * @brief Costruttore di TextEditor
 * @param parent Widget genitore. Viene usato internamente da Qt per gestione
 * di memoria, gerarchia, layout, propagazione di eventi, etc. (più informazioni reperibili nella documentazione officiale di Qt).
 *
 * Crea una finestra del editor di testo e stabilisce come verranno gestiti gli eventi GUI
 * connettendo dei *signal* (emessi da eventi) a *slot* (funzioni che gestiscono i suddetti eventi).
 */
TextEditor::TextEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TextEditor)
    , currentFile()
    , findAndReplace(FindAndReplaceDialog(this))
{
    ui->setupUi(this);

    QObject::connect(ui->actionNew, &QAction::triggered, this, &TextEditor::createNewFile);
    QObject::connect(ui->actionOpen, &QAction::triggered, this, &TextEditor::openFile);
    QObject::connect(ui->actionSave, &QAction::triggered, this, &TextEditor::saveFile);
    QObject::connect(ui->actionSaveAs, &QAction::triggered, this, &TextEditor::saveFileAs);
    QObject::connect(ui->actionFindAndReplace, &QAction::triggered, this, &TextEditor::showFindAndReplaceDialog);
    QObject::connect(&findAndReplace.findButton,  &QPushButton::clicked, this, &TextEditor::findWord);
    QObject::connect(&findAndReplace.replaceButton,  &QPushButton::clicked, this, &TextEditor::replaceWord);
}

/// distruttore di TextEditor
TextEditor::~TextEditor()
{
    delete ui;
}

/**
 * @brief Chiede al utente se vuole salvare in caso di modifiche non salvate.
 * @return restituisce **true** solo se o il salvataggio è andato a buon fine, o non era necessario
 * un salvataggio, oppure è stato richiesto di ignorare le modifiche
 *
 * Metodo privato chiamato da operazioni che possono portare alla perdita di modifiche non salvate del file di testo.
 * Da all'utente le opzioni di annullare l'operazione, eseguirla dopo un salvataggio o eseguirla ignorando le modifiche.
 */
bool TextEditor::promptForSave() {
    QMessageBox::StandardButton reply;
    if(ui->plainTextEdit->document()->isModified()) {
        reply =  QMessageBox::question(
            this,
            "Modifiche non salvate",
            "Alcune modifiche non sono state salvate. Vuoi salvare prima di chiudere?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save)
            saveFile();
    } else
        reply = QMessageBox::Discard;


    return reply == QMessageBox::Discard || !ui->plainTextEdit->document()->isModified();
}

/**
 * @brief Gestisce la chiusira della finestra chiamando promptForSave()
 * @param event Evento di chiusira.
 * @sa promptForSave()
 */
void TextEditor::closeEvent(QCloseEvent* event)
{
    if(promptForSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * @brief Crea un nuovo file vuoto.
 *
 * Gestisce salvataggi non eseguiti con promptForSave(), e se l'operazione non viene annullata, svuota il contenuto del
 * editor di testo e rimuove il nome del file. Funzionalmente l'editor di testo si comporterà come appena aperto.
 * In realtà il file nuovo viene nominato e scritto in memoria di massa solo dopo il salvataggio, non dopo questa operazione.
 *
 * @sa promptForSave()
 */
void TextEditor::createNewFile() {
    if(promptForSave()) {
        currentFile = "";
        ui->plainTextEdit->clear();
    }
}

/**
 * @brief Apre un file **.txt** o **.md** dal file system.
 *
 * Gestisce salvataggi non eseguiti con promptForSave(), e se l'operazione non viene annullata, apre un file di testo
 * dalla memoria di massa.
 *
 * @sa promptForSave()
 */
void TextEditor::openFile() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Selezionare un file:",
        QDir::homePath(),
        "File di testo o markdown (*.txt *.md)"
        );

    if(!fileName.isEmpty() && promptForSave()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Errore", "Non è stato possibile aprire il file");
        } else {
            QTextStream in(&file);
            ui->plainTextEdit->setPlainText(in.readAll());
            file.close();

            currentFile = fileName;
        }
    }
}

/**
 * @brief Salva il file attuale
 *
 * Salva, se possibile, il file attuale. Se il nome del file non è stato specificato, apre una finestra
 * di dialogo per scegliere dove salvare il file con saveFileAs().
 *
 * @sa saveFileAs()
 */
void TextEditor::saveFile() {
    if (currentFile.isEmpty()) {
        saveFileAs();
    } else {
        QFile file(currentFile);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Errore", "Non è stato possibile salvare il file");
        } else {
            QTextStream out(&file);
            out << ui->plainTextEdit->toPlainText();
            file.close();
            ui->plainTextEdit->document()->setModified(false);
        }
    }
}

/**
 * @brief Chiede al utente dove salvare il file attuale e lo salva.
 *
 * Apre una finestra di dialogo che permette al utente di decidere dove salvare il file in memoria e se con formato **.txt**
 * o **.md**. Dopo la scelta il file viene salvato con saveFile().
 *
 * @sa saveFile()
 */
void TextEditor::saveFileAs() {
    QString txtFilter = "File di testo (*.txt)";
    QString mdFilter = "Markdown (*.md)";
    QString selectedFilter = QString();
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Salva il file",
        QDir::homePath(),
        txtFilter + ";;" + mdFilter,
        &selectedFilter);

    if(!fileName.isEmpty()) {
        if (selectedFilter == txtFilter) {
            fileName += ".txt";
        } else if (selectedFilter == mdFilter) {
            fileName += ".md";
        }

        currentFile = fileName;
        saveFile();
    }
}

/**
 * @brief Rende visibile la finestra di dialogo per la ricerca e sostituzione di parole nel testo.
 *
 * @sa findWord(), replaceWord()
 */
void TextEditor::showFindAndReplaceDialog() {
    findAndReplace.dialog.resize(400, 200);
    findAndReplace.dialog.show();
}

/**
 * @brief Trova una parola nel testo secondo i parametri specificati.
 *
 * Trova la prossima posizione di una parola nel testo. Se la parola è trovata
 * la seleziona nel editot di testo, altrimenti avverte l'utente e posiziona il cursore a inizio pagina.
 */
void TextEditor::findWord() {
    QString searchText = findAndReplace.findEdit.text();

    if (!searchText.isEmpty()) {
        QTextCursor cursor = ui->plainTextEdit->textCursor();

        QString pattern;
        if (findAndReplace.wholeWordsCheckBox.isChecked()) {
            pattern = "\\b" + QRegularExpression::escape(searchText) + "\\b";
        } else {
            pattern = QRegularExpression::escape(searchText);
        }

        QTextDocument::FindFlags flags;
        if (findAndReplace.caseSensitiveCheckBox.isChecked())
            flags |= QTextDocument::FindCaseSensitively;

        QRegularExpression regex(pattern);
        cursor = ui->plainTextEdit->document()->find(regex, cursor, flags);

        if (!cursor.isNull()) {
            findAndReplace.messageLabel.setText("");
            ui->plainTextEdit->setTextCursor(cursor);
        } else {
            QTextCursor cursor = ui->plainTextEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);
            ui->plainTextEdit->setTextCursor(cursor);
            findAndReplace.messageLabel.setText("Il testo non è stato trovato.");
            QApplication::beep();
        }
    }
}

/**
 * @brief Sostituisce la selezione attuale con altra parola.
 *
 * Sostituisce la selezione attuale nel editor di testo con la stringa inserita nella locazione apposita nella dialog di ricerca.
 * Se nessuna parte del testo è selezionata, nessuna modifica viene apportata.
 */
void TextEditor::replaceWord() {
    QTextCursor cursor = ui->plainTextEdit->textCursor();
    if (cursor.hasSelection()) {
        cursor.insertText(findAndReplace.replaceEdit.text());
        ui->plainTextEdit->setTextCursor(cursor);
    }
}

/// costruttore della finestra di dialogo di ricerca.
TextEditor::FindAndReplaceDialog::FindAndReplaceDialog(TextEditor* window)
    : dialog(QDialog(window))
    , findEdit(QLineEdit(&dialog))
    , replaceEdit(QLineEdit(&dialog))
    , caseSensitiveCheckBox(QCheckBox("Considera maiuscole", &dialog))
    , wholeWordsCheckBox(QCheckBox("Solo parole intere", &dialog))
    , findButton(QPushButton("Trova", &dialog))
    , replaceButton(QPushButton("Sostituisci", &dialog))
    , messageLabel(QLabel(&dialog))
    , findLabel(QLabel("Cerca:", &dialog))
    , replaceLabel(QLabel("Sostituisci con:", &dialog))
    , mainLayout(QVBoxLayout(&dialog))
    , findLayout(QHBoxLayout())
    , replaceLayout(QHBoxLayout())
    , checkBoxLayout(QHBoxLayout())
    , buttonLayout(QHBoxLayout())
{
    dialog.setWindowTitle("Cerca e sostituisci");

    findLayout.addWidget(&findLabel);
    findLayout.addWidget(&findEdit);

    replaceLayout.addWidget(&replaceLabel);
    replaceLayout.addWidget(&replaceEdit);

    checkBoxLayout.addWidget(&caseSensitiveCheckBox);
    checkBoxLayout.addWidget(&wholeWordsCheckBox);

    buttonLayout.addWidget(&findButton);
    buttonLayout.addWidget(&replaceButton);

    mainLayout.addLayout(&findLayout);
    mainLayout.addLayout(&replaceLayout);
    mainLayout.addLayout(&checkBoxLayout);
    mainLayout.addLayout(&buttonLayout);

    mainLayout.addWidget(&messageLabel);
    messageLabel.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    messageLabel.setStyleSheet("color: red;");
}
