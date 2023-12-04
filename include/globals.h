/*******************************************************************************
 * Copyright (c) 2023 George Consultant Ltd.
 * richard.john.george.3@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/

// user tasks are used by the Esprossif ESPCON Non-OS SDK. 
// User tasks can be classified according to three priority levels: 0, 1, 2. 
// Priority level has the following order: 2>1>0. Non-OS SDK can only support 
// up to three tasks at a time. One priority level for one task.
//
// User tasks are normally used when a function cannot be called directly. To 
// create a user task, please refer to the API description of system_os_task() 
// in this document. For example, espconn_disconnect() API may not be called 
// from within an espconncallback, therefore a user task must be created within 
// the espconn callback to execute espconn_disconnect 

#ifndef USER_TASK_PRIO_0
#define USER_TASK_PRIO_0 0
#endif

#ifndef USER_TASK_PRIO_1
#define USER_TASK_PRIO_1 1
#endif

#ifndef USER_TASK_PRIO_2
#define USER_TASK_PRIO_2 2
#endif
