#include <atomic>
#include <list>
#include <iostream>
#include <string>
#include <thread>
#include <vector>


/*!
 * \brief An error-prone lock free queue implementation
 * \note The queue maintain two iterators: the head points to the element
 * before the oldest element, while the tail points to the element after the
 * newest.
 * 
 * The consumer only increments the head iterator and doesn't modify the queue.
 * The producer inserts elements and increments the tail. It also erases any
 * elements already read by the consumer.
 * These two methods never overlap with each other.
 * The two iterators never refer to the same position.
 * 
 * This implementation is still problematic because the iterators are accessed
 * from different threads while being modified in one of the threads. It has a
 * data race unless the iterators are protected by mutexes as iterators cannot
 * be atomic.
 * Maybe an alternative way is to use atomic pointers. Instead of using list
 * internally, it could be:
template<typename T>
class LockFreeQueue
{
public:
    // methods
private:
    struct Node
    {
        std::shared_ptr<T> data;
        std::atomic<Node*> next;
        Node(const T& d): data(std::make_shared<T>(d)) { }
    };
    std::atomic<Node*> _i_head;
    std::atomic<Node*> _i_tail;
};
 */
template<typename T>
class LockFreeQueue
{
public:
    // insert an initial element so that the two iterators don't point to the
    // same position.
    // _queue: [ ][T()][ ]
    //          ^       ^
    //          |       |
    //       _i_head  _i_tail
    LockFreeQueue()
    {
        _queue.push_back(T());
        _i_head = _queue.begin();
        _i_tail = _queue.end();
    }
    ~LockFreeQueue() = default;

    void Produce(const T& t)
    {
        _queue.push_back(t);
        _i_tail = _queue.end();
        _queue.erase(_queue.begin(), _i_head);
    }

    bool Consume(T& t)
    {
        auto i_first = _i_head;
        ++i_first;

        // if queue is not empty
        if (i_first != _i_tail)
        {
            _i_head = i_first;
            t = *_i_head;
            return true;
        }
        return false;
    }

    void Print()
    {
        auto i_first = _i_head;
        ++i_first;

        for (auto it = i_first; it != _i_tail; ++it)
        {
            std::cout << *it << ", ";
        }
        std::cout << "\n";
    }

private:
    std::list<T> _queue;
    typename std::list<T>::iterator _i_head, _i_tail;
};


int main()
{
    LockFreeQueue<int> queue;
    std::vector<std::thread> threads;
    int result{0};

    for (int i = 0; i < 10; ++i)
    {
        std::thread producer{&LockFreeQueue<int>::Produce, &queue, std::ref(i)};
        threads.push_back(std::move(producer));
        std::thread consumer{&LockFreeQueue<int>::Consume, &queue, std::ref(result)};
        threads.push_back(std::move(consumer));
    }

    for (auto& t : threads)
    {
        t.join();
    }

    std::cout << "Elements in queue: \n";
    queue.Print();

    return 0;
}
