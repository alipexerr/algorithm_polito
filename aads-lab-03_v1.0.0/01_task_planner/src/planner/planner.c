#include "planner/planner.h"

#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

PlannerResult planner_add_task(Planner *planner,
                               const char *identifier,
                               int due_date,
                               const char *label) {

    if (planner == NULL || identifier == NULL || label == NULL || *label == '\0') 
        return PLANNER_INVALID_DATA;

    if (strlen(identifier) > TASK_IDENTIFIER_CAPACITY - 1 || *identifier == '\0') 
        return PLANNER_INVALID_DATA;
    
    for (const unsigned char *character = (const unsigned char *)identifier;
        *character != '\0';
        character++) {
        if (isspace(*character))
            return PLANNER_INVALID_DATA;
    }
    
    if (strlen(label) > 63)
        return PLANNER_INVALID_DATA;

    for (const unsigned char *character = (const unsigned char *)label;
         *character != '\0';
         character++) {
        if (isspace(*character))
            return PLANNER_INVALID_DATA;
    }

    if (planner_find_task(planner, identifier) != NULL) 
        return PLANNER_DUPLICATE;

    Task *new_task = malloc(sizeof(Task));
    if (new_task == NULL) return PLANNER_NO_MEMORY;

    new_task->due_date = due_date;
    strcpy(new_task->identifier, identifier);
    new_task->label = strdup(label);
    
    if (new_task->label == NULL) {
        free(new_task);
        return PLANNER_NO_MEMORY;
    }
    Task *current = planner->pending_tasks;
    if (current == NULL) {
        new_task->next = NULL;
        planner->pending_tasks = new_task;
        planner->pending_task_count++;
        return PLANNER_OK;
    }

    while (current != NULL) {
        if (new_task->due_date < current->due_date && current == planner->pending_tasks) {
            new_task->next = current;
            planner->pending_tasks = new_task;
            planner->pending_task_count++;
            return PLANNER_OK;
        } else if (new_task->due_date == current->due_date) {
            if (strcmp(new_task->identifier, current->identifier) < 0) {
                new_task->next = current;
                if (current == planner->pending_tasks) {
                    planner->pending_tasks = new_task;
                } else {
                    Task *prev = planner->pending_tasks;
                    while (prev->next != current) {
                        prev = prev->next;
                    }
                    prev->next = new_task;
                }
                planner->pending_task_count++;
                return PLANNER_OK;
            } else if (current->next == NULL || new_task->due_date < current->next->due_date) {
                new_task->next = current->next;
                current->next = new_task;
                planner->pending_task_count++;
                return PLANNER_OK;
            }
        } else if (current->next == NULL && new_task->due_date > current->due_date) {
            current->next = new_task;
            new_task->next = NULL;
            planner->pending_task_count++;
            return PLANNER_OK;
        } else if (current->next == NULL || new_task->due_date < current->next->due_date) {
            new_task->next = current->next;
            current->next = new_task;
            planner->pending_task_count++;
            return PLANNER_OK;
        }
        current = current->next;
    }

    return PLANNER_INVALID_DATA; // if we reach here, we don't handled the insertion correctly, return an error
}

const Task *planner_find_task(const Planner *planner, const char *identifier) {
    if (planner == NULL || identifier == NULL) 
        return NULL;

    for (const Task *task = planner->pending_tasks; task != NULL ; task = task->next){
        if (strcmp(task->identifier, identifier) == 0) {
            return task;
        }
    }
    return NULL;
}

PlannerResult planner_remove_task(Planner *planner, const char *identifier) {
    if (planner == NULL || identifier == NULL) 
        return PLANNER_INVALID_DATA;
    for (Task *task = planner->pending_tasks; task != NULL; task = task->next) {
        if (strcmp(task->identifier, identifier) == 0) {
            if (task == planner->pending_tasks) {
                planner->pending_tasks = task->next;
            } else {
                Task *prev = planner->pending_tasks;
                while (prev->next != task) {
                    prev = prev->next;
                }
                prev->next = task->next;
            }
            free(task->label);
            free(task);
            planner->pending_task_count--;
            return PLANNER_OK;
        }
    }
    return PLANNER_NOT_FOUND;
}

PlannerResult planner_complete_task(Planner *planner, const char *identifier) {
    if (planner == NULL || identifier == NULL) 
        return PLANNER_INVALID_DATA;
    for (Task *task = planner->pending_tasks; task != NULL; task = task->next) {
        if (strcmp(task->identifier, identifier) == 0) {
            // Remove the task from pending tasks
            if (task == planner->pending_tasks) {
                planner->pending_tasks = task->next;
            } else {
                Task *prev = planner->pending_tasks;
                while (prev->next != task) {
                    prev = prev->next;
                }
                prev->next = task->next;
            }
            // Add the task to completed tasks
            task->next = planner->completed_tasks;
            planner->completed_tasks = task;
            planner->pending_task_count--;
            return PLANNER_OK;
        }
    }   
    return PLANNER_NOT_FOUND;
}

PlannerResult planner_undo_completion(Planner *planner) {
    if (planner == NULL) 
        return PLANNER_INVALID_DATA;
    if (planner->completed_tasks != NULL) {
        Task *task = planner->completed_tasks;
        // Add the task back to pending tasks
        char *identifier = task->identifier;
        int due_date = task->due_date;
        char *label = task->label;

        PlannerResult result = planner_add_task(planner, identifier, due_date, label);
        if (result == PLANNER_OK) {
            planner->completed_tasks = task->next;
            free(label); // Free the label since it's copied in planner_add_task
            free(task); // Free the task structure, but not the label since it's copied in planner
        }
        return result;
    }
    return PLANNER_NO_HISTORY;
}

void planner_destroy(Planner *planner) {
    for (Task *task = planner->pending_tasks; task != NULL; ){
        Task *next_task = task->next;
        free(task->label);
        free(task);
        task = next_task;
    }
    for (Task *task = planner->completed_tasks; task != NULL; ){
        Task *next_task = task->next;
        free(task->label);
        free(task);
        task = next_task;
    }
    planner->pending_tasks = NULL;
    planner->completed_tasks = NULL;
    planner->pending_task_count = 0;
}
