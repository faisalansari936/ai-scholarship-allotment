#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FIXED_SCHOLARSHIP 60000 
#define TARGET_POOL_SIZE 10000   
#define STARTING_ID 2026001

// Status flags for search query tracking
#define STATUS_GREEDY_REJECTED 0
#define STATUS_FUNDS_DEFERRED  1
#define STATUS_AWARDED         2

// ==========================================
// 1. DATA STRUCTURE DEFINITIONS
// ==========================================
typedef struct {
    int id;
    char name[50];
    float percentage;      
    float family_income;
    float deserving_score;  
    int status;            // AWARDED, DEFERRED, or REJECTED
    char details[100];     // Specific reason breakdown
} Student;

typedef struct {
    Student* data;         
    int size;              
    int capacity;          
} MaxHeap;

// Master Tracking Registry (Enables instant O(1) searches by ID offset)
Student master_registry[TARGET_POOL_SIZE];

// ==========================================
// 2. PRIORITY QUEUE MECHANICS
// ==========================================
MaxHeap* createMaxHeap(int initial_capacity) {
    MaxHeap* heap = (MaxHeap*)malloc(sizeof(MaxHeap));
    heap->capacity = initial_capacity;
    heap->size = 0;
    heap->data = (Student*)malloc(initial_capacity * sizeof(Student));
    return heap;
}

void swapStudents(Student* a, Student* b) {
    Student temp = *a;
    *a = *b;
    *b = temp;
}

void maxHeapify(MaxHeap* heap, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && heap->data[left].deserving_score > heap->data[largest].deserving_score)
        largest = left;

    if (right < heap->size && heap->data[right].deserving_score > heap->data[largest].deserving_score)
        largest = right;

    if (largest != index) {
        swapStudents(&heap->data[index], &heap->data[largest]);
        maxHeapify(heap, largest);
    }
}

void insertHeap(MaxHeap* heap, Student student) {
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;
        heap->data = (Student*)realloc(heap->data, heap->capacity * sizeof(Student));
    }
    heap->size++;
    int i = heap->size - 1;
    heap->data[i] = student;

    while (i != 0 && heap->data[(i - 1) / 2].deserving_score < heap->data[i].deserving_score) {
        swapStudents(&heap->data[i], &heap->data[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

Student extractMax(MaxHeap* heap) {
    if (heap->size <= 0) {
        Student empty = {0};
        return empty;
    }
    if (heap->size == 1) {
        heap->size--;
        return heap->data[0];
    }
    Student root = heap->data[0];
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    maxHeapify(heap, 0);
    return root;
}

float calculateDeservingScore(float percentage, float income) {
    float academic_component = percentage * 2.0f; 
    float financial_need_component = (250000.0f - income) / 2500.0f; 
    return academic_component + financial_need_component;
}

// ==========================================
// 3. GRAPHICAL AUDIT REPORT ENGINE
// ==========================================
void generateAuditReport(long long total_budget, long long funds_spent, int deferred_count, int rejected_count, int awarded_count) {
    int high_marks = 0, med_marks = 0, low_marks = 0;
    int low_income = 0, mid_income = 0, upper_income = 0;

    for (int i = 0; i < TARGET_POOL_SIZE; i++) {
        if (master_registry[i].status == STATUS_AWARDED) {
            if (master_registry[i].percentage >= 90.0f) high_marks++;
            else if (master_registry[i].percentage >= 75.0f) med_marks++;
            else low_marks++;

            if (master_registry[i].family_income < 100000.0f) low_income++;
            else if (master_registry[i].family_income < 180000.0f) mid_income++;
            else upper_income++;
        }
    }

    FILE* html_file = fopen("scholarship_audit_report.html", "w");
    if (html_file == NULL) {
        printf("❌ Critical Error: Could not write dashboard to disk.\n");
        return;
    }

    fprintf(html_file, "<!DOCTYPE html>\n<html>\n<head>\n<title>Scholarship Optimization Dashboard</title>\n");
    fprintf(html_file, "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>\n");
    fprintf(html_file, "<style>\n");
    fprintf(html_file, "  body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 40px; background-color: #f4f6f9; color: #333; }\n");
    fprintf(html_file, "  .container { max-width: 1200px; margin: 0 auto; background: white; padding: 30px; border-radius: 12px; box-shadow: 0 4px 20px rgba(0,0,0,0.05); }\n");
    fprintf(html_file, "  .header { text-align: center; border-bottom: 3px solid #003366; padding-bottom: 20px; margin-bottom: 30px; }\n");
    fprintf(html_file, "  h1 { color: #003366; margin: 0; font-size: 28px; font-weight: 700; text-transform: uppercase; }\n");
    fprintf(html_file, "  .kpi-container { display: grid; grid-template-columns: repeat(4, 1fr); gap: 20px; margin-bottom: 40px; }\n");
    fprintf(html_file, "  .kpi-card { background: #fff; border: 1px solid #e1e4e8; border-radius: 8px; padding: 20px; text-align: center; border-top: 4px solid #003366; }\n");
    fprintf(html_file, "  .kpi-val { font-size: 22px; font-weight: bold; color: #0056b3; margin-top: 5px; }\n");
    fprintf(html_file, "  .kpi-label { font-size: 11px; text-transform: uppercase; color: #6a737d; font-weight: 600; }\n");
    fprintf(html_file, "  .chart-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 30px; margin-bottom: 40px; }\n");
    fprintf(html_file, "  .chart-card { background: #fff; border: 1px solid #e1e4e8; padding: 20px; border-radius: 8px; }\n");
    fprintf(html_file, "  .chart-card h3 { margin-top: 0; color: #003366; border-bottom: 1px solid #eee; padding-bottom: 10px; }\n");
    fprintf(html_file, "  table { width: 100%%; border-collapse: collapse; margin-top: 20px; font-size: 13px; }\n");
    fprintf(html_file, "  th { background-color: #003366; color: white; text-align: left; padding: 12px; position: sticky; top: 0; }\n");
    fprintf(html_file, "  td { padding: 10px; border-bottom: 1px solid #dee2e6; }\n");
    fprintf(html_file, "  tr:nth-child(even) { background-color: #f8f9fa; }\n");
    fprintf(html_file, "  .badge-success { background: #d4edda; color: #155724; padding: 4px 8px; border-radius: 4px; font-weight: bold; font-size: 11px; }\n");
    fprintf(html_file, "  .badge-warning { background: #fff3cd; color: #856404; padding: 4px 8px; border-radius: 4px; font-weight: bold; font-size: 11px; }\n");
    fprintf(html_file, "  .badge-danger { background: #f8d7da; color: #721c24; padding: 4px 8px; border-radius: 4px; font-weight: bold; font-size: 11px; }\n");
    fprintf(html_file, "  .scroll-box { max-height: 500px; overflow-y: auto; border: 1px solid #e1e4e8; border-radius: 8px; }\n");
    fprintf(html_file, "</style>\n</head>\n<body>\n<div class='container'>\n");

    fprintf(html_file, "<div class='header'>\n  <h1>Smart Scholarship Optimization & Audit Dashboard</h1>\n");
    fprintf(html_file, "  <p style='color: #666;'>Full Data Verification Ledger (All 10,000 Rows Generated)</p>\n</div>\n");

    fprintf(html_file, "<div class='kpi-container'>\n");
    fprintf(html_file, "  <div class='kpi-card'><div class='kpi-label'>Total Budget Pool</div><div class='kpi-val'>₹%lld</div></div>\n", total_budget);
    fprintf(html_file, "  <div class='kpi-card'><div class='kpi-label'>Funds Spent</div><div class='kpi-val' style='color:#28a745;'>₹%lld</div></div>\n", funds_spent);
    fprintf(html_file, "  <div class='kpi-card'><div class='kpi-label'>Unallocated Leftover</div><div class='kpi-val' style='color:#dc3545;'>₹%lld</div></div>\n", total_budget - funds_spent);
    fprintf(html_file, "  <div class='kpi-card'><div class='kpi-label'>Deferred (No Funds)</div><div class='kpi-val' style='color:#fd7e14;'>%d Students</div></div>\n", deferred_count);
    fprintf(html_file, "</div>\n");

    fprintf(html_file, "<div class='chart-grid'>\n");
    fprintf(html_file, "  <div class='chart-card'><h3>Allotment By Academic Spectrum</h3><canvas id='marksChart'></canvas></div>\n");
    fprintf(html_file, "  <div class='chart-card'><h3>Allotment By Income Spectrum</h3><canvas id='incomeChart'></canvas></div>\n");
    fprintf(html_file, "</div>\n");

    fprintf(html_file, "<h3>Master Registration Registry Ledger</h3>\n");
    fprintf(html_file, "<div class='scroll-box'>\n<table>\n<thead>\n<tr>\n");
    fprintf(html_file, "  <th>Student ID</th><th>Applicant Name</th><th>Marks</th><th>Annual Income</th><th>Deserving Score</th><th>Status Outcome</th><th>Details / Reason Log</th>\n");
    fprintf(html_file, "</tr>\n</thead>\n<tbody>\n");

    for (int i = 0; i < TARGET_POOL_SIZE; i++) {
        fprintf(html_file, "<tr>\n");
        fprintf(html_file, "  <td>%d</td><td>%s</td><td>%.1f%%</td><td>₹%.0f</td><td>%.2f</td>\n",
                master_registry[i].id, master_registry[i].name, master_registry[i].percentage, 
                master_registry[i].family_income, master_registry[i].deserving_score);
        
        if (master_registry[i].status == STATUS_AWARDED) {
            fprintf(html_file, "  <td><span class='badge-success'>APPROVED</span></td><td><span style='color:green; font-weight:600;'>₹%d Disbursed</span></td>\n", FIXED_SCHOLARSHIP);
        } else if (master_registry[i].status == STATUS_FUNDS_DEFERRED) {
            fprintf(html_file, "  <td><span class='badge-warning'>DEFERRED</span></td><td style='color:#856404;'>%s</td>\n", master_registry[i].details);
        } else {
            fprintf(html_file, "  <td><span class='badge-danger'>REJECTED</span></td><td style='color:#721c24;'>%s</td>\n", master_registry[i].details);
        }
        fprintf(html_file, "</tr>\n");
    }
    fprintf(html_file, "</tbody>\n</table>\n</div>\n");

    // CHART SCRIPT GENERATION
    fprintf(html_file, "<script>\n");
    fprintf(html_file, "const ctxMarks = document.getElementById('marksChart').getContext('2d');\n");
    fprintf(html_file, "new Chart(ctxMarks, { type: 'bar', data: {\n");
    fprintf(html_file, "  labels: ['Highest Marks (90-100%%)', 'Medium Marks (75-89%%)', 'Lowest Eligible (61-74%%)'],\n");
    fprintf(html_file, "  datasets: [{ label: 'Recipients Count', data: [%d, %d, %d], backgroundColor: ['#003366', '#0056b3', '#33a3ff'] }]\n", high_marks, med_marks, low_marks);
    fprintf(html_file, "}, options: { responsive: true } });\n");

    fprintf(html_file, "const ctxIncome = document.getElementById('incomeChart').getContext('2d');\n");
    fprintf(html_file, "new Chart(ctxIncome, { type: 'pie', data: {\n");
    fprintf(html_file, "  labels: ['Lowest Income (<1L)', 'Mid Income (1L-1.8L)', 'Upper Eligible (1.8L-2.5L)'],\n");
    fprintf(html_file, "  datasets: [{ data: [%d, %d, %d], backgroundColor: ['#28a745', '#ffc107', '#dc3545'] }]\n", low_income, mid_income, upper_income);
    fprintf(html_file, "}, options: { responsive: true } });\n");
    fprintf(html_file, "</script>\n</div>\n</body>\n</html>\n");
    fclose(html_file);

    printf("\n📊 AUDIT REPORT UPDATED: 'scholarship_audit_report.html' written to folder.\n");
}

// ==========================================
// 4. INTERACTIVE USER SEARCH INTERFACE
// ==========================================
void runSearchTerminal() {
    int search_id;
    printf("\n=======================================================\n");
    printf("🔍 STUDENT SCHOLARSHIP SEARCH TERMINAL\n");
    printf("=======================================================\n");
    printf("Type an ID to search, or type '0' to close the terminal.\n");

    while (1) {
        printf("\nEnter Student ID Number (ex: %d): ", STARTING_ID);
        scanf("%d", &search_id);

        if (search_id == 0) {
            printf("Exiting Search Interface. Program complete.\n");
            break;
        }

        // Validate index parameters
        int target_index = search_id - STARTING_ID;
        if (target_index < 0 || target_index >= TARGET_POOL_SIZE) {
            printf("❌ Error: Invalid Student ID. Range is from %d to %d\n", STARTING_ID, STARTING_ID + TARGET_POOL_SIZE - 1);
            continue;
        }

        Student res = master_registry[target_index];
        printf("\n-------------------------------------------------------\n");
        printf("ID Ref:      %d\n", res.id);
        printf("Full Name:   %s\n", res.name);
        printf("Academics:   %.1f%%\n", res.percentage);
        printf("Income Check:₹%.2f\n", res.family_income);
        printf("Rank Score:  %.2f\n", res.deserving_score);
        printf("-------------------------------------------------------\n");
        printf("FINAL STATUS: ");

        if (res.status == STATUS_AWARDED) {
            printf("🎉 SUCCESS! Approved for Allocation\n");
            printf("Amount Issued: ₹%d Disbursed into Bank Account.\n", FIXED_SCHOLARSHIP);
        } else if (res.status == STATUS_FUNDS_DEFERRED) {
            printf("⚠️ ELIGIBLE BUT DEFERRED\n");
            printf("Reason Code: %s\n", res.details);
        } else {
            printf("❌ APPLICATION REJECTED\n");
            printf("Reason Code: %s\n", res.details);
        }
        printf("-------------------------------------------------------\n");
    }
}

// ==========================================
// 5. MAIN EXECUTIVE WORKFLOW
// ==========================================
int main() {
    srand(time(NULL)); 
    long long total_budget;
    int processed_count = 0;
    int greedy_rejected = 0;
    char choice;

    printf("=== SMART SCHOLARSHIP OPTIMIZATION SYSTEM (10K ENGINE) ===\n");
    printf("Enter Total Allocatable Fund Budget: ₹");
    scanf("%lld", &total_budget);

    MaxHeap* queue = createMaxHeap(TARGET_POOL_SIZE);

    printf("\n--- Manual Entry Mode Activated ---\n");
    while (processed_count < TARGET_POOL_SIZE) {
        Student s;
        s.id = STARTING_ID + processed_count;

        printf("\n[Manual Application Input #%d]\n", processed_count + 1);
        printf("Enter First Name (No spaces): ");
        scanf("%49s", s.name);
        printf("Enter Academic Score Percentage (0 - 100): ");
        scanf("%f", &s.percentage);
        printf("Enter Annual Family Income (INR): ");
        scanf("%f", &s.family_income);

        // Run Baseline Greedy Filter
        if (s.percentage > 60.0f && s.family_income < 250000.0f) {
            s.deserving_score = calculateDeservingScore(s.percentage, s.family_income);
            s.status = STATUS_FUNDS_DEFERRED; // Default flag before optimization checks budget limits
            strcpy(s.details, "Passed screening thresholds; processing priority index.");
            insertHeap(queue, s);
            printf("✅ Approved for verification queue (Rank Score: %.2f)\n", s.deserving_score);
        } else {
            s.deserving_score = 0.0f;
            s.status = STATUS_GREEDY_REJECTED;
            if (s.percentage <= 60.0f && s.family_income >= 250000.0f) {
                strcpy(s.details, "Failed both filters: Marks <= 60% AND Income >= 2.5L Lakhs.");
            } else if (s.percentage <= 60.0f) {
                strcpy(s.details, "Academic marks failure: Scored below required 60% threshold.");
            } else {
                strcpy(s.details, "Economic threshold failure: Family Income exceeds maximum ₹2,50,000 bracket.");
            }
            greedy_rejected++;
            printf("❌ Filter Reject: %s\n", s.details);
        }

        // Commit profile metadata to database registry table
        master_registry[processed_count] = s;
        processed_count++;

        printf("\nDo you want to auto-generate the remaining data up to 10,000 entries? (y/n): ");
        scanf(" %c", &choice);

        if (choice == 'y' || choice == 'Y') {
            printf("\n🤖 Simulating data generation up to %d entries...\n", TARGET_POOL_SIZE);
            while (processed_count < TARGET_POOL_SIZE) {
                Student auto_s;
                auto_s.id = STARTING_ID + processed_count;
                sprintf(auto_s.name, "Profile_%d", auto_s.id);
                
                auto_s.percentage = 45.0f + ((float)(rand() % 550) / 10.0f);        
                auto_s.family_income = 40000.0f + ((float)(rand() % 410000) / 10.0f); 

                if (auto_s.percentage > 60.0f && auto_s.family_income < 250000.0f) {
                    auto_s.deserving_score = calculateDeservingScore(auto_s.percentage, auto_s.family_income);
                    auto_s.status = STATUS_FUNDS_DEFERRED;
                    strcpy(auto_s.details, "Passed screening thresholds; processing priority index.");
                    insertHeap(queue, auto_s);
                } else {
                    auto_s.deserving_score = 0.0f;
                    auto_s.status = STATUS_GREEDY_REJECTED;
                    if (auto_s.percentage <= 60.0f && auto_s.family_income >= 250000.0f) {
                        strcpy(auto_s.details, "Failed both filters: Marks <= 60% AND Income >= 2.5L.");
                    } else if (auto_s.percentage <= 60.0f) {
                        strcpy(auto_s.details, "Academic score below 60%.");
                    } else {
                        strcpy(auto_s.details, "Family income exceeds maximum ₹2,50,000.");
                    }
                    greedy_rejected++;
                }
                master_registry[processed_count] = auto_s;
                processed_count++;
            }
            break; 
        }
    }

    // --- EXECUTE RESOURCE ALLOCATION ENGINE ---
    int awarded_count = 0;
    long long funds_spent = 0;

    while (queue->size > 0) {
        Student top_candidate = extractMax(queue);
        int reg_index = top_candidate.id - STARTING_ID;

        if (funds_spent + FIXED_SCHOLARSHIP > total_budget) {
            // All remaining items left inside queue are marked as deferred due to exhaustion of funds
            master_registry[reg_index].status = STATUS_FUNDS_DEFERRED;
            strcpy(master_registry[reg_index].details, "Approved as deserving, but funding dried up due to lower rank placement.");
            
            while (queue->size > 0) {
                Student rem = extractMax(queue);
                int rem_idx = rem.id - STARTING_ID;
                master_registry[rem_idx].status = STATUS_FUNDS_DEFERRED;
                strcpy(master_registry[rem_idx].details, "Approved as deserving, but funding dried up due to lower rank placement.");
            }
            break;
        }

        master_registry[reg_index].status = STATUS_AWARDED;
        strcpy(master_registry[reg_index].details, "Scholarship fully approved and disbursed.");
        awarded_count++;
        funds_spent += FIXED_SCHOLARSHIP;
    }

    int deferred_count = TARGET_POOL_SIZE - greedy_rejected - awarded_count;

    // Trigger full visualization generator
    generateAuditReport(total_budget, funds_spent, deferred_count, greedy_rejected, awarded_count);

    // Launch Real-Time ID Search Terminal Interface Engine
    runSearchTerminal();

    free(queue->data);
    free(queue);
    return 0;
}