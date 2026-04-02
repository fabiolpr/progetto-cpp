/**
 *  @file queue.h
 *   @brief File header contenente la una libreria per code FIFO.
 *
 *   Contiene le dichiarazioni e definizioni template
 *   della classe Queue e di funzioni globali collegate ad essa.
*/

#ifndef QUEUE_H
#define QUEUE_H

#include <stdexcept>


/**
 * @brief Classe che definisce code di reference.
 *
 * @tparam T Tipo di dato contenuto della coda.
 *
 * Una coda FIFO di tipo generico. I dati inseriti in coda *non* vengono copiati, la coda
 * immagazinerà solo una reference al dato originale.
 */
template <typename T>
class Queue {
    /**
     * @brief transformif è dichiarate friend.
     *
     * Alla funzione transformif è concesso accesso a i membri private della classe Queue.
     *
     * @sa transformif(Queue<T>& queue, bool checkCondition(const T&), F& transform)
     */
    template <typename U, typename F>
    friend void transformif(Queue<U>& queue, bool checkCondition(const U&), F& transform);
    //forward declaration di node
    struct Node;
public:

    /**
     * @brief Classe per eccezioni causate da accesso erroneo a code vuote.
     *
     * Alcune operazioni sono possibili su tutte le code tranne quelle vuote.
     * In quei casi limite la suddetta eccezione verrà sollevata.
     */
    class EmptyQueueError : public std::runtime_error {
    public:
        EmptyQueueError();
    };

    /**
     * @brief Classe che definisce iteratori di sola lettura per la classe Queue.
     *
     * Funzionalmente si comporta come un **forward iterator** di tipo **const**. L'accesso per iteratore segue l'ordinamento
     * FIFO, icrementare un iteratore valido che non punta all'ultimo elemento lo sposterà su un elemento meno anziano.
     * Per mantenere gli iteratori veloci e conformi alla libreria standard, stati o operazioni erronei
     * non verranno segnalati o gestiti dall'iteratore stesso. Spetta al programmatore assicurarsi che l'utilizzo sia corretto.
     * ***Attenzione: Se il dato puntato dal iteratore è stato rimosso dalla coda, usare l'iteratore risulta in undefined behavior.
     * Dereferenziare o accedere ai membri di un iteratore posizionato dopo l'ultimo elemento della coda (come quello restituito da cend()) risulterà in
     * ***undefined behavior***. Incrementare l'iteratore più di un elemento dopo la fine della coda
     * (ancora dopo l'iteratore restituito da cend()) risulta in undefined behavior.***
     */
    class Iterator : public std::iterator<std::forward_iterator_tag, const T> {
        friend class Queue<T>;

    public:

        const T& operator*() const;
        const T* operator->() const;
        //undefined behavior se value è nullptr
        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;

    private:

        Iterator(Node* node);
        Node* node;
    };

    Queue();
    Queue(const Queue<T>& other);
    ~Queue();

    unsigned int getLength() const;
    bool has(const T& value) const;

    void enqueue(T& value);
    template <typename StartIterator, typename EndIterator>
    void enqueue(StartIterator start, EndIterator end);
    T& dequeue();

    void setFirst(const T& value);
    void setLast(const T& value);
    T& getFirst() const;
    T& getLast() const;
    void deleteFirst();

    Iterator cbegin() const;
    Iterator cend() const;

    Queue<T>& operator=(const Queue<T>& other);

private:

    /**
     * @brief Struct usato internamente per costruire liste dinamiche.
     */
    struct Node {
        Node(T* value, Node* next);
        Node(T& value, Node* next);

        /// Ll valore del nodo.
        T* const value;
        /// Puntatore al prossimo nodo della lista dinamica.
        Node* next;
    };

    void checkIfEmpty() const;

    /// Nodo dummy che funge da sentinella per gli iteratori.
    Node* const endNode;
    /// Puntatore all'primo elemento della coda.
    Node* head;
    /// Puntatore all'ultimo elemento della coda.
    Node* tail;
    /// Lunghezza della coda.
    unsigned int length;
};

//definizioni template della classe Queue

/**
 * @brief Costruttore senza parametri di Queue.
 *
 * Crea una nuova istanza di Queue senza elementi.
 */
template <typename T>
Queue<T>::Queue()
    : endNode(new Node(nullptr, nullptr))
    , head(new Node(nullptr, endNode))
    , tail(head)
    , length(0)
{}

/**
 * @brief Copy costructor di Queue.
 *
 * @param other Altra istanza di queue dalla quale copiare.
 *
 * Crea una nuova istanza di Queue contenente gli stessi elementi contenuti nella coda other.
 * Internamente, la nuova coda punterà agli stessi oggetti dell'originale, ma le due code terranno traccia di cosa contengono separatamente.
 * (Esempio: dequeue su una coda non comporta dequeue su l'altra, ma setFirst su una comporta modifiche per il prossimo dato in uscita di entrambe)
 */
template <typename T>
Queue<T>::Queue(const Queue<T>& other)
    : endNode(new Node(nullptr, nullptr))
    , head(new Node(nullptr, endNode))
    , length(other.length)
{
    Node* node = head;
    Node* otherNode = other.head->next;
    while(otherNode != other.endNode) {
        //copia solo il puntatore di value
        node->next = new Node(otherNode->value, endNode);
        node = node->next;
        otherNode = otherNode->next;
    }
    tail = node;
}

/**
 * @brief Distruttore di Queue.
 *
 * Non comporta la distruzione degli oggetti di tipo T inseriti nella coda. Eventuali **delete** o deallocazioni
 * di oggetti interni dovranno essere fatte manualmente.
 */
template <typename T>
Queue<T>::~Queue() {
    while(head != nullptr) {
        Node* node = head->next;
        delete head;
        head = node;
    }
}

///@return Restituisce un unsigned int corrispondente al numero di elementi in coda.
template <typename T>
inline unsigned int Queue<T>::getLength() const {
    return length;
}

/**
 * @brief Controlla se un valore è presente in coda.
 * @param value Valore da cercare.
 * @return true se il valore è trovato, falso altrimenti.
 *
 * La reference passata al metodo verrà comparata ad ogni elemento in coda con
 * l'operatore <b>==</b> fino a trovate o una corrispondenza o il termine della coda.
 * Se il paragone con un elemento ha restituito **true**, il metodo **has** restituirà a sua volta **true**.
 * Se il termine della coda viene raggiunto senza successo, il metodo **has** restituirà **false**.
 */
template <typename T>
bool Queue<T>::has(const T& value) const {
    Node* node = head->next;
    bool found = false;
    while(!found && node != endNode) {
        found = *node->value == value;
        node = node->next;
    }
    return found;
}

/**
 * @brief Inserisce un singolo elemento.
 * @param value Reference da inserire in coda.
 *
 * Aggiunge un nuovo elemento come ultimo della coda.
 */
template <typename T>
void Queue<T>::enqueue(T& value) {
    Node* const node = new Node(value, endNode);
    tail->next = node;
    tail = node;
    ++length;
}

/**
 * @brief Inserisce valori in un range.
 *
 *  @tparam StartIterator Iteratore generico che dereferenzia a dati di tipo T.
 *  @tparam EndIteratorator Iteratore generico che dereferenzia a dati di tipo T.
 *  @param start Iteratore di inizio.
 *  @param end Iteratore di fine.
 *
 *  Dati due iteratori di inizio e fine sequenza, aggiungerà ogni elemento puntato dal iteratore **start**
 *  e incrementerà il suddetto fino a quando **start == end**. I valori aggiunti saranno nel range [**start**, **end**).
 */
template <typename T>
template <typename StartIterator, typename EndIterator>
void Queue<T>::enqueue(StartIterator start, EndIterator end) {
    while (start != end) {
        enqueue(*start);
        ++start;
    }
}

/**
 * @brief Rimuove un elemento dalla coda e lo restituisce.
 *
 * @return L'elemento che è stato rimosso.
 */
template <typename T>
T& Queue<T>::dequeue() {
    checkIfEmpty();
    //il primo nodo non è la "vera" head, ma un nodo sentinella
    const Node* oldNode = head->next;
    head->next = oldNode->next;
    T& value = *oldNode->value;
    delete oldNode;
    --length;
    if(length < 1)
        tail = head;
    return value;
}

/**
 * @brief Modifica il prossimo dato in uscita.
 *
 * @param value Il valore che verrà usato per la sovrascrizione.
 *
 * Modifica il dato che è prossimo ad uscire dalla coda con il dato **value** usando l'operatore di assegmanento ( <b>=</b> ).
 */
template <typename T>
void Queue<T>::setFirst(const T& value) {
    checkIfEmpty();
    *head->next->value = value;
}

/**
 * @brief Modifica l'ultimo dato inserito.
 *
 * @param value Il valore che verrà usato per la sovrascrizione.
 *
 * Modifica il dato che è stato inserito per ultimo in coda con il dato **value** usando l'operatore di assegmanento ( <b>=</b> ).
 */
template <typename T>
void Queue<T>::setLast(const T& value) {
    checkIfEmpty();
    *tail->value = value;
}

/**
 * @brief Restituisce il prossimo dato in uscita.
 *
 * @return Il valore che sarà il prossimo ad uscire.
 *
 * Il valore restituito è lo stesso ottenuto con dequeue, ma in questo caso non viene rimosso.
 */
template <typename T>
T& Queue<T>::getFirst() const {
    checkIfEmpty();
    return *head->next->value;
}

/**
 * @brief Restituisce l'ultimo dato inserito.
 *
 * @return Il valore che è stato inserito per ultimo.
 */
template <typename T>
T& Queue<T>::getLast() const {
    checkIfEmpty();
    return *tail->value;
}

/**
 * @brief Rimuove il prossimo elemento e lo distrugge.
 *
 * Rimuove il prossimo elemento in uscita come dequeue, ma invece di restituirlo lo dealloca con l'operatore **delete**.
 */
template <typename T>
void Queue<T>::deleteFirst() {
    const T& value = dequeue();
    delete &value;
}

/**
 * @brief Restituisce un iteratore **const** di inizio coda.
 *
 * @return iteratore **const** di tipo Iterator.
 *
 * Restituisce un iteratore **const** di tipo Queue<T>::Iterator che punta al elemento più anziano della coda.
 *
 * @sa Queue<T>::Iterator
 */
template <typename T>
inline typename Queue<T>::Iterator Queue<T>::cbegin() const {
    return Iterator(head->next);
}

/**
 * @brief Restituisce un iteratore **const** dopo la fine della coda.
 *
 * @return iteratore **const** di tipo Iterator.
 *
 * Restituisce un iteratore **const** di tipo Queue<T>::Iterator che punta un elemento dopo l'ultimo elemento inserito in coda.
 *
 * @sa Queue<T>::Iterator
 */
template <typename T>
inline typename Queue<T>::Iterator Queue<T>::cend() const {
    return Iterator(endNode);
}

/**
 * @brief Operatore di assegnamento <b>=</b>.
 *
 * @param other Istanza a destra del operatore.
 *
 * Dopo l'assegnamento, la coda conterrà gli stessi oggetti contenuti in **other**.
 * Gli oggetti interni alla coda verranno copiati per puntatore, non per valore.
 */
template <typename T>
Queue<T>& Queue<T>::operator=(const Queue<T>& other){
    Queue<T> temporary(other);
    length = temporary.length;
    Node* newNode = temporary.head;
    temporary.head = head;
    head = newNode;

    newNode = temporary.tail;
    temporary.tail = tail;
    temporary.tail->next = temporary.endNode;
    tail = newNode;
    tail->next = endNode;
    return *this;
}

/// Solleva un eccezione di tipo EmptyQueueError se la coda è vuota.
template <typename T>
void Queue<T>::checkIfEmpty() const {
    if(length < 1)
        throw EmptyQueueError();
}

//definizioni template della classe EmptyQueueError


/// Costruttore del tipo di eccezione EmptyQueueError.
template <typename T>
Queue<T>::EmptyQueueError::EmptyQueueError() : std::runtime_error("Attempted to access elements of an empty queue.") {}


//definizioni template della classe Node

/**
 * @brief Costruttore per nuove istanze di Node da reference.

 * @param value Il dato che verrà contenuto dal nodo.
 * @param next Il successore del nodo che stiamo costruendo.
 */
template <typename T>
Queue<T>::Node::Node(T& value, Node* next) : value(&value), next(next) {}

/**
 * @brief Costruttore per nuove istanze di Node da pointer.

 * @param value Il dato che verrà contenuto dal nodo.
 * @param next Il successore del nodo che stiamo costruendo.
 */
template <typename T>
Queue<T>::Node::Node(T* value, Node* next) : value(value), next(next) {}


//definizioni template della classe Iterator

/**
 * @brief costruttore per la classe Iterator.
 *
 * @param node Nodo che contiene il dato che verrà puntato dal iteratore.
 */
template <typename T>
Queue<T>::Iterator::Iterator(Node* node) : node(node) {}

/**
 * @brief operatore di dereferenziamento <b>*</b>.
 *
 * @return Reference **const** al dato di tipo T puntato dal iteratore.
 */
template <typename T>
inline const T& Queue<T>::Iterator::operator*() const {
    return *node->value;
}

/**
 * @brief Operatore di accesso da puntatore.
 *
 * @return Puntatore **const** al dato di tipo T puntato dal iteratore.
 */
template <typename T>
inline const T* Queue<T>::Iterator::operator->() const {
    return node->value;
}

/// Operatore di incremento postfisso **++**.
template <typename T>
inline typename Queue<T>::Iterator& Queue<T>::Iterator::operator++() {
    node = node->next;
    return *this;
}

/// Operatore di incremento prefisso **++**.
template <typename T>
inline typename Queue<T>::Iterator Queue<T>::Iterator::operator++(int) {
    const Iterator instance = *this;
    node = node->next;
    return instance;
}

/**
 * @brief Operatore di confronto **==**.
 *
 * @param other Altro iteratore con cui fare il confronto.
 *
 * @return **true** solo se i due operatori puntano allo stesso
 * oggetto nella stessa posizione della stessa lista.
 */
template <typename T>
inline bool Queue<T>::Iterator::operator==(const Iterator& other) const {
    return node == other.node;
}

/**
 * @brief Operatore di confronto **!=**.
 *
 * @param other Altro iteratore con cui fare il confronto.
 *
 * @return **true** solo se il confronto con operatore <b>==</b> restituisce **false**.
 *
 * @sa operator==()
 */
template <typename T>
inline bool Queue<T>::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}


//funzioni globali

/**
     * @brief Applica una trasformazione ad ogni elemento che soddisfa una condizione.
     *
     * @relates Queue
     * @tparam T Tipo di dato della coda.
     * @tparam F Tipo del funtore.
     * @param queue La coda da trasformare.
     * @param checkCondition Puntatore a funzione che accetta una const reference a un elemento della coda e restituisce una bool. Funge da predicato.
     * @param transform Funtore che accetta una const reference a un elemento della coda e applica una trasformazione su di esso.
     *
     * Per ogni elemento della coda, se la condizione verificata da checkCondition(const T&) è vera, verrà
     * applicata una trasformazione con il funtore transform.
     */
template <typename T, typename F>
void transformif(Queue<T>& queue, bool checkCondition(const T&), F& transform) {
    typename Queue<T>::Node* node = queue.head->next;
    while(node != queue.endNode) {
        T& value = *node->value;
        if(checkCondition(value))
            transform(value);
        node = node->next;
    }
}

#endif
