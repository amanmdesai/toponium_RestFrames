import os
from fpdf import FPDF
import math

def make_two_column_pdf_checklist(output_pdf="checklist.pdf"):
    current_folder = os.getcwd()
    files = sorted(f for f in os.listdir(current_folder) if os.path.isfile(os.path.join(current_folder, f)))

    # Remove ".pdf" extension from display names
    display_files = [f[:-4] if f.lower().endswith(".pdf") else f for f in files]

    num_files = len(display_files)
    half = math.ceil(num_files / 2)
    left_column = display_files[:half]
    right_column = display_files[half:]

    pdf = FPDF()
    pdf.add_page()
    pdf.set_font("Arial", size=11)

    pdf.cell(0, 10, "Checklist for Current Folder", ln=True)
    pdf.ln(5)

    col_width = 90  # half of A4 width (180 mm)
    row_height = 8
    checkbox = "[ ]"

    for i in range(half):
        left_item = f"{checkbox} {left_column[i]}" if i < len(left_column) else ""
        right_item = f"{checkbox} {right_column[i]}" if i < len(right_column) else ""

        pdf.cell(col_width, row_height, left_item, border=0)
        pdf.cell(col_width, row_height, right_item, border=0)
        pdf.ln(row_height)

    pdf.output(output_pdf)
    print(f"Checklist PDF created: {output_pdf}")

# Run it
make_two_column_pdf_checklist()
