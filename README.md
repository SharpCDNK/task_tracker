# Task Tracker

Sample solution for the [task-tracker](https://roadmap.sh/projects/task-tracker) challenge from [roadmap.sh](https://roadmap.sh/).

## How to run

Clone the repository and run the following command:

```bash
git clone https://github.com/SharpCDNK/task_tracker.git
cd task_tracker
```

Run the following command to build and run the project:

```bash
mkdir -p build
cd build

cmake -DCMAKE_TOOLCHAIN_FILE=../gcc13.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -S .. -B .
cmake --build .

# To add a task
./task-cli add "Buy groceries"

# To update a task
./task-cli update 1 "Buy groceries and cook dinner"

# To delete a task
./task-cli delete 1

# To mark a task as in progress/done/todo
./task-cli mark-in-progress 1
./task-cli mark-done 1
./task-cli mark-todo 1

# To list all tasks
./task-cli list
./task-cli list done
./task-cli list todo
./task-cli list in-progress
```
