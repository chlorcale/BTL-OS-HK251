#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t *q)
{
    if (q == NULL)
        return 1;
    return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
    /* TODO: put a new process to queue [q] */
    // Kiểm tra xem hàng đợi có đầy không
    if (q->size >= MAX_QUEUE_SIZE)
        return;
    
    // Thêm tiến trình vào cuối hàng đợi
    q->proc[q->size] = proc;
    q->size++;
}

struct pcb_t *dequeue(struct queue_t *q)
{
    /* TODO: return a pcb whose prioprity is the highest
     * in the queue [q] and remember to remove it from q
     * */
    if (q == NULL || q->size == 0)
        return NULL;

    // Tìm process có độ ưu tiên cao nhất (prio lớn nhất)
    // Lưu ý: Nếu quy ước OS của bạn là số nhỏ = ưu tiên cao hơn, hãy đổi dấu '>' thành '<'
    int max_prio_idx = 0;
    for (int i = 1; i < q->size; i++)
    {
        // So sánh độ ưu tiên. Giả sử prio càng lớn thì độ ưu tiên càng cao.
        if (q->proc[i]->prio > q->proc[max_prio_idx]->prio)
        {
            max_prio_idx = i;
        }
    }

    struct pcb_t *ret_proc = q->proc[max_prio_idx];

    // Dời các phần tử phía sau lên để lấp chỗ trống
    for (int i = max_prio_idx; i < q->size - 1; i++)
    {
        q->proc[i] = q->proc[i + 1];
    }

    // Xóa phần tử cuối cùng (đã được dời lên) và giảm kích thước
    q->proc[q->size - 1] = NULL;
    q->size--;

    return ret_proc;
}

struct pcb_t *purgequeue(struct queue_t *q, struct pcb_t *proc)
{
    /* TODO: find and remove a pcb from queue [q] */
    if (q == NULL || q->size == 0 || proc == NULL)
        return NULL;

    int pos = -1;
    // Tìm vị trí của proc trong hàng đợi
    for (int i = 0; i < q->size; i++)
    {
        if (q->proc[i] == proc)
        {
            pos = i;
            break;
        }
    }

    // Nếu không tìm thấy
    if (pos == -1)
        return NULL;

    // Dời các phần tử để xóa proc
    for (int i = pos + 1; i < q->size; i++)
    {
        q->proc[i - 1] = q->proc[i];
    }

    q->proc[q->size - 1] = NULL;
    q->size--;

    return proc;
}