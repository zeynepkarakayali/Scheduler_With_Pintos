# PINTOS Project 1 - Schheduler:
Pintos is a simple operating system framework for the 80x86 architecture. It supports kernel threads, loading and running user programs, and a file system, but it implements all of these in a very simple way. We will also add a virtual memory implementation.

Pintos could, theoretically, run on a regular IBM-compatible PC but we will run Pintos projects in a system simulator, that is, a program that simulates an 80x86 CPU and its peripheral devices accurately enough that unmodified operating systems and software can run under it. In this proejct we will use the *Bochs* and *QEMU* simulators.

## History of Pintos
Pintos was originally developed at Stanford by Ben Pfaff blp@cs.stanford.edu to substitute for the old OS course project Nachos. After more than a decade of iterations, Pintos has been adopted by over fifty institutes as the OS course project, including Stanford, UC Berkeley, Carnegie Mellon, Johns Hopkins, and so on. You can read the original Pintos paper (Yes, they even write a paper for it !) to learn the details of Pintos' design philosophy and its comparison with other instructional operating system kernels, e.g., JOS, Nachos, GeekOS, and so on.

------------
## Preliminaires
1. **Understanding Existing Code**: The first step in working with the Pintos thread system is to comprehend the initial code, which already includes thread creation, scheduling, and synchronization primitives.

2. **Reading Source Code**: One approach to understanding the code is to read through it, adding print statements or using a debugger to observe its behavior and order of execution.

3. **Thread Creation and Execution**: Threads in Pintos are like mini-programs, each running a function provided to `thread_create()`. When a thread is scheduled for the first time, it starts from the beginning of its function and terminates when the function returns.

4. **Scheduler**: Only one thread runs at a time, chosen by the scheduler. If no thread is ready to run, the "idle" thread runs.

5. **Synchronization**: Synchronization primitives like semaphores can cause context switches when a thread needs to wait for another.

6. **Context Switch Mechanics**: Context switches are handled by assembly code in "threads/switch.S", saving the state of the currently running thread and restoring the state of the thread being switched to.

7. **Debugging with GDB**: Using the GDB debugger, one can trace through a context switch, understanding why and how `switch_threads()` behaves differently when called and when returning.

8. **Stack Management**: Each thread has a fixed-size execution stack, and care must be taken to avoid stack overflow, such as by avoiding large non-static local variables.

----------------
## Debugging vs Testing:
1. **Reproducibility in Debugging**: When debugging code, it's beneficial for the program to behave the same on multiple runs, enabling consistent observations. Pintos uses Bochs simulator with reproducibility by default, ensuring the same behavior across runs.

2. **Reproducibility Constraints**: Reproducibility requires identical conditions for each run, including command-line arguments, disk contents, and simulator version. Any deviation, such as keyboard input, can disrupt reproducibility.

3. **Testing Thread Synchronization**: Reproducibility poses a challenge for testing thread synchronization because timer interrupts and thread switches occur predictably. This limits the usefulness of running tests multiple times.

4. **Introducing Jitter**: To address testing challenges, Pintos adds a feature called "jitter" to Bochs, making timer interrupts occur at random but predictable intervals. Tests can then be run with different seed values for varied timer behavior, increasing confidence in code correctness.

5. **Realistic Timings**: Bochs can be set to realistic timings with the `-r` option, where delays approximate real-time durations. However, this mode sacrifices reproducibility.

6. **QEMU as an Alternative**: QEMU is a faster alternative to Bochs but only supports real-time simulation and lacks a reproducible mode.

Overall, the text highlights the importance of reproducibility in debugging, the challenges it poses for testing thread synchronization, and the solutions provided by Pintos through jitter and realistic timing options in Bochs.

----------------
## Alarm Clock
In this section, the task is to reimplement the `timer_sleep()` function in `devices/timer.c` to avoid busy waiting. Currently, the provided implementation busy waits by spinning in a loop, continuously checking the current time and calling `thread_yield()` until the specified time has elapsed.

The objective is to modify `timer_sleep()` to suspend the execution of the calling thread until the system time has advanced by at least a specified number of timer ticks. The thread need not wake up exactly after the specified number of ticks, but rather should be placed back on the ready queue after the correct amount of time has passed.

The function `timer_sleep()` is particularly useful for threads operating in real-time scenarios, such as blinking a cursor once per second.

--------------
## Priority Scheduling and Priority Donation in Pintos:

Pintos implements priority scheduling, where threads with higher priority preempt those with lower priority.

- **Thread Priorities**: Priorities range from `PRI_MIN (0)` to `PRI_MAX (63)`, with lower numbers indicating lower priorities. The initial thread priority is set when a thread is created, defaulting to `PRI_DEFAULT (31)`. These priority macros are defined in `threads/thread.h`.

- **Scheduler Behavior**: When a thread of higher priority becomes ready, the currently running thread immediately yields the CPU. Similarly, when threads are waiting for synchronization primitives like locks, semaphores, or condition variables, the highest priority waiting thread is awakened first.

- **Priority Donation**: To mitigate priority inversion issues, threads can donate their priority to others while waiting for resources. For instance, a high priority thread waiting on a lock held by a low priority thread can temporarily donate its priority to the lower priority thread until it releases the lock.

- **Handling Priority Donation**:
  - Multiple Donations: Handle situations where multiple threads donate their priorities to a single thread.
  - Nested Donation: Support nested priority donation, where threads waiting on locks held by other waiting threads also inherit higher priorities. Implement a reasonable limit on nested donation depth, such as 8 levels.
  - Implementation Scope: Priority donation must be implemented for locks, but not necessarily for other synchronization constructs like semaphores or condition variables. However, priority scheduling should be implemented in all cases.

Overall, the priority scheduling and priority donation mechanisms in Pintos aim to ensure fair execution of threads and mitigate priority inversion issues through the thoughtful management of thread priorities.

--------------

## Advanced Scheduler
In this section, we will implement a multilevel feedback queue scheduler inspired by the 4.4BSD scheduler. This scheduler aims to reduce the average response time for running jobs by maintaining several queues of ready-to-run threads with different priorities and using a "round robin" approach to select threads from the highest-priority non-empty queue. 

### Niceness
In the Niceness subsection, thread priority in the Pintos scheduler is dynamically determined based on a formula, while each thread also has an integer "nice" value that influences its priority. 
- Positive nice values decrease priority, allowing threads to yield CPU time, while negative values tend to increase priority, potentially taking CPU time from other threads.
- Functions `thread_get_nice()` and `thread_set_nice()` are provided to retrieve and modify a thread's nice value, respectively, with priority recalculated accordingly.

### Calculating Priority
In the section on Calculating Priority, the Pintos scheduler determines thread priority using a formula that incorporates recent CPU usage and the thread's "nice" value. Priority is recalculated periodically based on recent CPU time changes, ensuring fairness in CPU allocation and preventing starvation by adjusting priority based on recent CPU usage.

**The formula:**

$priority = PRIMAX - (\frac{recent_{cpu}}{4}) - (nice \times 2)$


### Calculating Recent CPU
This section describes the calculation of the `recent_cpu` value for each thread in the Pintos operating system. It utilizes an exponentially weighted moving average formula to measure how much CPU time each process has received recently. The `recent_cpu` value is updated with each timer interrupt and recalculated once per second for every thread, taking into account the system load average and the thread's nice value. The `thread_get_recent_cpu()` function is provided to retrieve the current thread's `recent_cpu` value.

**Exponentially weighted moving average:**

$x(0) = f(0);$

$x(t) = a \times x(t - 1) + f(t);$

$a = \frac{k}{k + 1}$


where x(t) is the moving average at integer time t >= 0, f(t) is the function being
averaged, and k > 0 controls the rate of decay. We can iterate the formula over a few
steps as follows:

$x(1) =f(1);$

$x(2) =a \times f(1) + f(2);$

...

$x(5) =a^4 \times f(1) + a^3 \times f(2) + a^2 \times f(3) + a \times f(4) + f(5):$

**Recent CPU Formula:**
The initial value of recent_cpu is 0 in the first thread created, or the parent’s value in other new threads. Each time a timer interrupt occurs, recent_cpu is incremented by 1 for the running thread only, unless the idle thread is running. In addition, once per second the value of recent_cpu is recalculated for every thread (whether running, ready, or blocked), using this formula:


$recent_{cpu}=\frac{(2 \times load_{avg})}{(2 \times load_{avg} + 1) \times recent_{cpu}}+nice$

where load_avg is a moving average of the number of threads ready to run.

### Calculating Load Average
This section explains the calculation of the system load average (`load_avg`) in the Pintos operating system. Similar to `recent_cpu`, `load_avg` is also calculated using an exponentially weighted moving average formula. It estimates the average number of threads ready to run over the past minute and is updated once per second based on the number of threads that are either running or ready to run at the time of update.

$load_{avg}=\frac{59}{60} \times load_{avg} + \frac{1}{60} \times ready_{threads}$
