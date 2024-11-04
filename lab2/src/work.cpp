#include <iostream>
#include <algorithm> 
#include <queue>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>

const int numThread = 5;

int findGreatestCommonDivisor(int a, int b);
int _findGreatestCommonDivisor(int a, int b, int start, int end);


struct Task {
    int taskNumber;
    int x;
    int y;
     void execute() {
        std::string tmp1 = "output";
        std::string tmp2 = ".txt";
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        std::string filename = tmp1+oss.str()+tmp2; // Имя файла
        std::ofstream outfile;

        outfile.open(filename, std::ios::app);
        if (!outfile) {
        std::cerr << "Can`t open file: " << filename << std::endl;
        return ; 
        }
        outfile << "Задача "<< taskNumber << ": result - " << findGreatestCommonDivisor(x,y) << std::endl;


        std::cout << "Task " << taskNumber << " is being executed." << std::endl;
        // std::this_thread::sleep_for(std::chrono::seconds(1)); // Симуляция работы
    }
};

// Описание очереди задач
class TaskQueue {
public:
    void push(Task task) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(task);
        cv_.notify_one(); // Уведомляем один поток, который ждет
    }

    Task pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); }); // Ожидаем, пока очередь не станет непустой
        Task task = queue_.front();
        queue_.pop();
        return task;
    }

    bool empty(){
        return queue_.empty();
    };

    void notify(){
        cv_.notify_one();
    };
    
    

private:
    std::queue<Task> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};


int findGreatestCommonDivisor(int a, int b) {
    
    int maxNum = std::max(a, b);

    return _findGreatestCommonDivisor( a,  b, 0, maxNum);
 
}

int _findGreatestCommonDivisor(int a, int b, int start, int end) {
    
    for (int i = end; i >= start+1; --i) {
        if (a % i == 0 && b % i == 0) { 
            return i; 
        }
    }

    return 1; 
}

// std::queue<Task> readTasksFromFile(const std::string &filename) {
//     std::queue<Task> tasks;
//     std::ifstream file(filename); 

//     if (!file.is_open()) {
//         std::cerr << "Can`t open file: " << filename << std::endl;
//         return tasks;
//     }

    
//     while (std::getline(file, line)) { 
//         std::istringstream iss(line); 
//         Task task;

        
//         if (iss >> task.taskNumber) {
        
//             std::string ignore;
//             std::getline(iss, ignore, '.');
//             iss >> task.x >> task.y;
//             tasks.push(task); 
//         }
//     }

    
//     return tasks;
// }


// Функция потока-поставщика
void producer(TaskQueue &taskQueue) {
    std::string filename = "tasks.txt";
    std::ifstream file(filename); 

    if (!file.is_open()) {
        std::cerr << "Can`t open file: " << filename << std::endl;
        throw std::invalid_argument("");
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line); 
        Task task;

        
        if (iss >> task.taskNumber) {
        
            std::string ignore;
            std::getline(iss, ignore, '.');
            iss >> task.x >> task.y;
            taskQueue.push(task);
            std::cout << "Produced task " << task.taskNumber << std::endl;
            // std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
        }
    }
    file.close(); 
    // // std::this_thread::sleep_for(std::chrono::seconds(10));
    // while(!taskQueue.empty()){
    //     std::cout << "Задача " << taskQueue.pop().taskNumber << " потеряна!";
    // }
    
}

void consumer(TaskQueue &taskQueue) {
    bool is_we_work = true;

    while (is_we_work) {
        Task task = taskQueue.pop();
        task.execute();  
        std::this_thread::sleep_for(std::chrono::milliseconds(500));    
        if(taskQueue.empty()){
            is_we_work = false;
        }
    }
    
}



int main() {
    
     TaskQueue taskQueue;

    std::thread producerThread(producer, std::ref(taskQueue));
    std::thread consumerThread1(consumer, std::ref(taskQueue));
    std::thread consumerThread2(consumer, std::ref(taskQueue));
    std::thread consumerThread3(consumer, std::ref(taskQueue));
    std::thread consumerThread4(consumer, std::ref(taskQueue));

    producerThread.join();
    consumerThread1.join();
    consumerThread2.join();
    consumerThread3.join();
    consumerThread4.join();

    
    return 0;
}
