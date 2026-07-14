# ai-scholarship-allotment
The Smart Scholarship Optimization & Audit Dashboard automates scholarship allocation for up to 10,000 applications. It replaces manual spreadsheets with a transparent, scalable system that optimizes budget use, ensures policy compliance, maintains audit trails, reduces errors, and improves efficiency through real-time analytics.

The Smart Scholarship Optimization & Audit Dashboard is an interactive, single-page web app built to automate and audit student scholarship distributions. It is designed to handle 10,000 applicant records entirely in the browser with zero lag.

🌟 Key Features Smart Matching: Filters out ineligible students instantly (e.g., family income over ₹2.5L or marks under 60%) and calculates a custom merit score for the rest.

Optimal Budgeting: Uses a greedy knapsack algorithm to award ₹60,000 scholarships to the most deserving candidates first until the budget runs out.

O(1) Instant Search Portal: Students can type in their 7-digit ID and instantly see if they were Approved, Deferred (qualified but the budget ran out), or Rejected (with the exact reason).

Executive Graphs: Generates dynamic, interactive charts (using Chart.js) showing the distribution of funds by academic marks and family income.

Full Accountability Ledger: Features a scrollable list containing every single one of the 10,000 profiles for transparent auditing.

🛠️ Tech Stack Frontend: HTML5, Tailwind CSS

Charts & Visuals: Chart.js

Engine & Logic: Native JavaScript (no backend server or external database requi
