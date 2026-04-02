/**
 * @file main.cpp
 * @brief File contenente test vari.
 *
 * File contenente un main che esegue test su oggetti di tipo Queue, oltre
 * a funzioni e struct usati dal main per compiere il suo obbiettivo.
*/

#include <iostream>
#include <array>
#include <algorithm>
#include <cassert>
#include "queue.h"

using namespace std;

/// lunghezza delle due std::array usate nei test.
#define ARRAY_LENGTH 10

/**
 * @brief Struct usato per testare le code su tipo complessi.
 */
struct Person {
    string name;
    int id;
    bool employed;

    /**
     * @brief Costruttore di Person.
     *
     * @param name Una stringa a scelta.
     * @param id Identifica "univocamente" lo struct (in pratica nulla vieta **id** duplicati).
     * @param employed Un valore di bool a scelta.
     */
    Person(string name, int id, bool employed) : name(name), id(id), employed(employed) {}

    /**
     * @brief Operatore di assegnamento **=**.
     *
     * Sovrascrive i valori di **name** e **employed** dell'istanza corrente
     * con valori uguali a quelli del istanza a destra del operatore. **id** non viene modificato.
     */
    void operator=(const Person& other) {
        name = other.name;
        employed = other.employed;
    }

    /**
     * @brief Operatore di confronto <b>==</b>.
     *
     * @return **true** se i due oggetti comparati hanno **id** uguale.
     */
    bool operator==(const Person& other) const {
        return id == other.id;
    }
};

/**
 * @brief Funtore che moltiplica un int.
 */
struct MultiplyByValueFunctor {
    /// valore per il quale la reference passata viene moltiplicata.
    static const int multiplier = 10;

    /// Moltiplica ad una **int** passata come reference un valore usando l'operatore <b>*=</b>.
    void operator()(int& n) {
        n *= multiplier;
    }
};

/**
 * @brief Funtore che somma un valore a un int.
 */
struct SumValueFunctor {
    /// Valore che viene sommato all'int passata.
    static const int toSum =  5;

    /// Somma ad una **int** passata come reference un valore usando l'operatore <b>+=</b>.
    void operator()(int& n) {
        n += toSum;
    }
};

/**
 * @brief Funtore che "assume" oggetti Person.
 */
struct HireFunctor {
    /// Numero di posizioni di lavoro aperte.
    unsigned short positions = 3;

    /**
     * @brief Assume persone finchè non finiscono i posti.
     *
     * Se **positions** è maggiore di 0, imposta il membro **employed** dell'oggetto di tipo Person passato
     * come reference a true e decrementa il numero di posti disponibili.
     */
    void operator()(Person& p) {
        if(positions > 0) {
            p.employed = true;
            --positions;
        }
    }
};

/**
 * @brief Accorcia il membro **name** del oggetto Person passato a 5 caratteri e concatena "...".
 */
struct ShortenNameFunctor {
    void operator()(Person& p) {
        p.name = p.name.substr(0, 5) + "...";
    }
};

/// Predicato che verifica se un int è pari.
bool isEven(const int& n) {
    return n % 2 == 0;
}
/// Predicato che verifica se un int ha valore in [1, 5].
bool isInRange(const int& n) {
    return 1 <= n && n <= 5;
}

/// Predicato che verifica se il membro **id** di un oggetto Person passato è maggiore o uguale a 5.
bool isIdAcceptable(const Person& p) {
    return p.id >= 5;
}

/// Predicato che verifica se il membro **name** di un oggetto Person passato è maggiore di 6.
bool isNameLong(const Person& p) {
    return p.name.length() > 6;
}

/**
 * @brief Verifica che la classe Queue solleva eccezioni quando deve.
 * @param toTest La istanza di Queue sulla quale fare test.
 * @param element Elemento di tipo corrispondente alla coda necessario per alcuni test.
 */
template <typename T>
void testQueueExceptions(Queue<T>& toTest, T& element) {
    cout << "test eccezioni:" << endl;
    try {
        toTest.dequeue();
        assert(false);
    } catch(const typename Queue<T>::EmptyQueueError& e) {
        cout << e.what() << endl;
    }

    try {
        toTest.getFirst();
        assert(false);
    } catch(const typename Queue<T>::EmptyQueueError& e) {
        cout << e.what() << endl;
    }

    try {
        toTest.getLast();
        assert(false);
    } catch(const typename Queue<T>::EmptyQueueError& e) {
        cout << e.what() << endl;
    }

    try {
        toTest.setFirst(element);
        assert(false);
    } catch(const typename Queue<T>::EmptyQueueError& e) {
        cout << e.what() << endl;
    }

    try {
        toTest.setLast(element);
        assert(false);
    } catch(const typename Queue<T>::EmptyQueueError& e) {
        cout << e.what() << endl;
    }
    cout << "ok" << endl << '\n';
}

/**
 * @brief Fa test per la funzione dequeue.
 *
 * @param toTest La istanza di Queue da testare.
 * @param elements Una std::array con tipo uguale a toTest e lunghezza uguale alla macro **ARRAY_LENGTH**.
 *
 * Suppone che nella istanza di Queue passata siano presenti tutti e solo gli elementi dell'array **elements**.
 * Al termine della funzione la coda passata sarà vuota.
 */
template <typename T>
void testDequeue(Queue<T>& toTest, array<T, ARRAY_LENGTH>& elements) {
    cout << "test di dequeue:" << endl;
    for(size_t i = 0; i < ARRAY_LENGTH ; i++) {
        T& object = toTest.dequeue();
        unsigned int length = toTest.getLength();
        cout << "lunghezza: " << length << flush;
        assert(&object == &elements[i]);
        assert(length == ARRAY_LENGTH  - i - 1);
        cout << " (lunghezza e ordine ok)" << endl;
    }
    cout << '\n';
}

/**
 * @brief Test generici per istanze di Queue.
 *
 * @param toTest La istanza di Queue da testare.
 * @param elements Una std::array con tipo uguale a toTest e lunghezza uguale alla macro **ARRAY_LENGTH**

* Esegue test su istanze di Queue che possono essere eseguiti a prescindere dal tipo di dati che la coda accetta.
 * A fine esecuzione **toTest** conterrà i primi 5 elementi di **elements**.
 */
template <typename T>
void commonTests(Queue<T>& toTest, array<T, ARRAY_LENGTH>& elements) {
    unsigned int length = toTest.getLength();
    assert(length == 0);
    testQueueExceptions(toTest, elements[0]);

    cout << "test di enqueue con una reference come argument:" << endl;
    for(size_t i = 0; i < ARRAY_LENGTH ; i++) {
        toTest.enqueue(elements[i]);
        length = toTest.getLength();
        cout << "lunghezza: " << length << flush;
        assert(length == i + 1);
        cout << " (ok)" << endl;
    }
    cout << '\n';

    testDequeue(toTest, elements);

    testQueueExceptions(toTest, elements[0]);

    cout << "test di enqueue con iteratori:" << endl;
    toTest.enqueue(elements.begin(), elements.cend());
    length = toTest.getLength();
    cout << "lunghezza: " << length << flush;
    assert(length == ARRAY_LENGTH );
    cout << " (ok)" << endl << '\n';

    cout << "test della classe interna Queue<T>::Iterator:" << endl;
    typename Queue<T>::Iterator iter1 = toTest.cbegin();
    typename Queue<T>::Iterator iter2 = toTest.cend();
    assert(distance(iter1, iter2) == ARRAY_LENGTH );
    cout << "distance(InputIt first, InputIt last) ok" << endl;
    advance(iter1, 3);
    assert(*iter1 == elements[3]);
    cout << "advance(InputIt& it, Distance n) ok" << endl;
    typename Queue<T>::Iterator iter3 = find(iter1, iter2, elements[3]);
    assert(*iter3 == elements[3]);
    cout << "find(InputIt first, InputIt last, const T& value) ok" << endl;
    assert(iter3 == iter1);
    assert(iter3++ != ++iter1);
    assert(iter3 == iter1);
    ++iter1;
    assert(++iter3 == iter1);
    cout << "test degli operatori ok" << endl << '\n';

    testDequeue(toTest, elements);

    assert(toTest.cbegin() == toTest.cend());
    cout << "return di .cbegin() == return di .cend() ok" << endl << '\n';

    typename std::array<T, ARRAY_LENGTH>::iterator iter4 = elements.begin();
    advance(iter4, 5);
    toTest.enqueue(elements.begin(), iter4);
    for(int i = 0; i < 5; i++)
        assert(toTest.has(elements[i]));
    for(int i = 5; i < ARRAY_LENGTH; i++)
        assert(!toTest.has(elements[i]));
    cout << "test di .has(T& value) ok" << endl << '\n';

    Queue<T> copyQueue = Queue<T>(toTest);
    typename Queue<T>::Iterator iter5 = toTest.cbegin();
    typename Queue<T>::Iterator iter6 = copyQueue.cbegin();
    typename Queue<T>::Iterator iter7 = toTest.cend();
    while(iter5 != iter7) {
        assert(iter5 != iter6);
        assert((*iter5) == *iter6);
        ++iter5;
        ++iter6;
    }
    assert(iter7 != copyQueue.cend());
    cout << "test di copy constructor e operatore di assegnamento ok\n";
    cout << "(l'operatore di assegnamento usa il copy constructor internamente)" << endl << '\n';
}

/**
 * @brief Punto di ingresso del programma.
 *
 * Esegue test vari su due istanze di Queue. Una coda accetta reference ad un tipo complesso, l'altra invece reference a **int**.
 */
int main(int argc, char* argv[]) {
    Queue<Person> structQueue;
    Queue<int> intQueue;

    array<Person, ARRAY_LENGTH> structElements = {
        Person("maria", 0, false),
        Person("laura", 1, false),
        Person("marco", 2, false),
        Person("giovanni", 3, false),
        Person("giulia", 4, false),
        Person("anna", 5, false),
        Person("luca", 6, false),
        Person("andrea", 7, false),
        Person("francesca", 8, false),
        Person("matteo", 9, false)
    };

    array<int, ARRAY_LENGTH> intElements = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    //test di deallocazione (da controllare con valgrind)
    Person* dynamicStruct = new Person("giovanni", 3, true);
    structQueue.enqueue(*dynamicStruct);
    structQueue.deleteFirst();
    int* dynamicInt = new int(3);
    intQueue.enqueue(*dynamicInt);
    intQueue.deleteFirst();

    Person p("giovanni", 3, true);
    structQueue.enqueue(p);
    structQueue = Queue<Person>();
    int n = 3;
    intQueue.enqueue(n);
    intQueue = Queue<int>();

    cout << "TEST SU STRUCT DI TIPO \"Person\"" << endl << '\n';

    commonTests(structQueue, structElements);

    Person wrongId("giorgio", 3, true);
    assert(structQueue.has(wrongId));
    cout << "test del uso di == in .has() ok" <<endl;
    structQueue.setFirst(structElements[5]);
    structQueue.setLast(structElements[6]);
    assert(&structQueue.getFirst() == &structElements[0]);
    assert(&structQueue.getLast() == &structElements[4]);
    assert(structElements[0].id == 0);
    assert(structElements[4].id == 4);
    assert(structElements[0].name == structElements[5].name);
    assert(structElements[4].name == structElements[6].name);
    cout << "test di getter e setter per la testa e la coda ok" << endl << '\n';
    structElements[0].id = 0;
    structElements[4].id = 4;

    typename Queue<Person>::Iterator iter1 = structQueue.cbegin();
    assert((*iter1).id == iter1->id);
    cout << "test di accesso a membri da iteratore con -> ok" << endl << '\n';

    cout << "test di transformif:" << endl;
    structQueue.enqueue(structElements.begin() + 5, structElements.cend());
    HireFunctor h;
    ShortenNameFunctor sh;
    transformif(structQueue, isIdAcceptable, h);
    transformif(structQueue, isNameLong, sh);
    typename Queue<Person>::Iterator iter2 = structQueue.cend();
    while(iter1 != iter2) {
        cout << iter1->name << ", " << iter1->id << ", " << iter1->employed << endl;
        ++iter1;
    }
    cout << '\n';


    cout << "----------------------------------\n";
    cout << "TEST SU VARIABILI PRIMITIVE DI TIPO \"int\"" << endl << '\n';

    commonTests(intQueue, intElements);

    n = 3;
    assert(intQueue.has(n));
    cout << "test del uso di == in .has() ok" <<endl;
    intQueue.setFirst(intElements[5]);
    intQueue.setLast(intElements[6]);
    assert(&intQueue.getFirst() == &intElements[0]);
    assert(&intQueue.getLast() == &intElements[4]);
    assert(intElements[0] == intElements[5]);
    assert(intElements[4] == intElements[6]);
    cout << "test di getter e setter per la testa e la coda ok" << endl << '\n';
    intElements[0] = 0;
    intElements[4] = 4;

    cout << "test di transformif:" << endl;
    intQueue.enqueue(intElements.begin() + 5, intElements.cend());
    MultiplyByValueFunctor m;
    SumValueFunctor su;
    transformif(intQueue, isInRange, m);
    transformif(intQueue, isEven, su);
    typename Queue<int>::Iterator iter3 = intQueue.cbegin();
    typename Queue<int>::Iterator iter4 = intQueue.cend();
    while(iter3 != iter4) {
        cout << *iter3 << ", ";
        ++iter3;
    }
    cout << endl << '\n';

    cout <<"------------fine dei test------------" << endl;
    return 0;
}
