#include "../src/cbl.h"
#include "main.h"

static void example1(const char *path)
{
	BasicExcel xls;

	//创建一个工作表并返回工作表的指针
	xls.New(1);
	BasicExcelWorksheet* sheet = xls.GetWorksheet(0);

	XLSFormatManager fmt_mgr(xls);

	ExcelFont font_bold;
	font_bold._weight = FW_BOLD;

	CellFormat fmt_bold(fmt_mgr);
	fmt_bold.set_font(font_bold);

	int col, row = 0;

	for (col = 0; col < 10; ++col) {
		BasicExcelCell *cell = sheet->Cell(row, col);

		cell->Set("TITLE");
		cell->SetFormat(fmt_bold);
	}

	while (++row < 4) {
		for (col = 0; col < 10; ++col) {
			sheet->Cell(row, col)->Set("text");
		}
	}

	++row;

	ExcelFont font_red_bold;
	font_red_bold._weight = FW_BOLD;
	font_red_bold._color_index = EGA_RED;

	CellFormat fmt_red_bold(fmt_mgr, font_red_bold);
	fmt_red_bold.set_color1(COLOR1_PAT_SOLID);			
	fmt_red_bold.set_color2(MAKE_COLOR2(EGA_BLUE, 0));

	CellFormat fmt_green(fmt_mgr, ExcelFont().set_color_index(EGA_GREEN));

	for (col = 0; col < 10; ++col) {
		BasicExcelCell *cell = sheet->Cell(row, col);

		cell->Set("xxx");
		cell->SetFormat(fmt_red_bold);

		cell = sheet->Cell(row, ++col);
		cell->Set("yyy");
		cell->SetFormat(fmt_green);
	}

	xls.SaveAs(path);
}

static void example2(const char *path)
{
	BasicExcel xls;

	xls.New(1);
	BasicExcelWorksheet* sheet = xls.GetWorksheet(0);

	XLSFormatManager fmt_mgr(xls);

	ExcelFont font_header;
	font_header.set_weight(FW_BOLD);
	font_header.set_underline_type(EXCEL_UNDERLINE_SINGLE);
	font_header.set_font_name(L"Times New Roman");
	font_header.set_color_index(EGA_BLUE);
	font_header._options = EXCEL_FONT_STRUCK_OUT;

	CellFormat fmt_header(fmt_mgr, font_header);
	fmt_header.set_rotation(30);

	int row = 0;

	for (int col = 0; col < 10; ++col) {
		BasicExcelCell* cell = sheet->Cell(row, col);

		cell->Set("TITLE");
		cell->SetFormat(fmt_header);
	}

	char buffer[100];

	while (++row < 10) {
		for (int col = 0; col < 10; ++col) {
			sprintf(buffer, "text %d/%d", row, col);
			sheet->Cell(row, col)->Set(buffer);
		}
	}

	xls.SaveAs(path);
}

static void example3(const char *path)
{
	BasicExcel xls;

	xls.New(1);
	BasicExcelWorksheet* sheet = xls.GetWorksheet(0);

	XLSFormatManager fmt_mgr(xls);
	CellFormat fmt(fmt_mgr);
	BasicExcelCell* cell;

	//row 1
	fmt.set_format_string(XLS_FORMAT_INTEGER);
	cell = sheet->Cell(0, 0);
	cell->Set(1.);
	cell->SetFormat(fmt);

	fmt.set_format_string(XLS_FORMAT_DECIMAL);
	cell = sheet->Cell(0, 1);
	cell->Set(2.);
	cell->SetFormat(fmt);

	fmt.set_format_string(XLS_FORMAT_DATE);
	fmt.set_font(ExcelFont().set_weight(FW_BOLD));
	cell = sheet->Cell(0, 2);
	cell->Set("03.03.2000");
	cell->SetFormat(fmt);

	//row 2
	fmt.set_font(ExcelFont().set_weight(FW_NORMAL));
	fmt.set_format_string(XLS_FORMAT_GENERAL);
	cell = sheet->Cell(1, 0);
	cell->Set("normal");
	cell->SetFormat(fmt);

	fmt.set_format_string(XLS_FORMAT_TEXT);
	cell = sheet->Cell(1, 1);
	cell->Set("Text");
	cell->SetFormat(fmt);

	fmt.set_format_string(XLS_FORMAT_GENERAL);
	fmt.set_font(ExcelFont().set_weight(FW_BOLD));
	cell = sheet->Cell(1, 2);
	cell->Set("bold");
	cell->SetFormat(fmt);

	xls.SaveAs(path);
}

static void example_read_write(const char *from, const char *to)
{
	cout << "read " << from << endl;
	BasicExcel xls(from);

	XLSFormatManager fmt_mgr(xls);
	BasicExcelWorksheet* sheet = xls.GetWorksheet(0);

	CellFormat fmt_general(fmt_mgr);

	fmt_general.set_format_string("0.000");

	for(int y = 0; y < 2; ++y) {
		for(int x = 0; x < 2; ++x) {
			cout << y << "/" << x;

			BasicExcelCell* cell = sheet->Cell(y, x);
			CellFormat fmt(fmt_mgr, cell);
			const Workbook::Font& font = fmt_mgr.get_font(fmt);
			
			string font_name = stringFromSmallString(font.name_);
			cout << "  font name: " << font_name;

			const wstring& fmt_string = fmt.get_format_string();
			cout << "  format: " << narrow_string(fmt_string);

			cell->SetFormat(fmt_general);
			cout << endl;
		}
	}

	cout << "write: " << from << endl;
	xls.SaveAs(to);
}

static void example4(const char *path)
{
	BasicExcel xls;

	xls.New(1);
	BasicExcelWorksheet* sheet = xls.GetWorksheet(0);

	XLSFormatManager fmt_mgr(xls);

	char buffer[100];
	int i = 0;

	for (int row = 0; row < 8; ++row) {
		int color = i++;
		int height = 100 * i;

		sprintf(buffer, "Times New Roman %d", height/20);

		ExcelFont font;
		font.set_color_index(color);
		font.set_height(height);
		font.set_font_name(L"Times New Roman");

		CellFormat fmt(fmt_mgr, font);
		fmt.set_background(MAKE_COLOR2(EGA_MAGENTA,0));

		BasicExcelCell* cell = sheet->Cell(row, 0);
		cell->Set(buffer);
		cell->SetFormat(fmt);
	}

	xls.SaveAs(path);
}

static void copy_sheet(const char *from, const char *to)
{
	BasicExcel xls;

	xls.Load(from);
	xls.SaveAs(to);
}

static void write_big_sheet(const char *path, const int row_max, const int col_max)
{
	BasicExcel xls;
	char buffer[16];

	xls.New(1);
	BasicExcelWorksheet* sheet = xls.GetWorksheet(0);

	XLSFormatManager fmt_mgr(xls);


	// Create a table containing header row and column in bold.
	ExcelFont font_bold;
	font_bold._weight = FW_BOLD;

	CellFormat fmt_bold(fmt_mgr);
	fmt_bold.set_font(font_bold);

	int col, row;

	BasicExcelCell* cell = sheet->Cell(0, 0);
	cell->Set("Row / Column");
	cell->SetFormat(fmt_bold);

	for (col = 1; col <= col_max; ++col) {
		cell = sheet->Cell(0, col);

		sprintf(buffer, "Column %d", col);
		cell->Set(buffer);
		cell->SetFormat(fmt_bold);
	}

	for (row = 1; row <= row_max; ++row) {
		cell = sheet->Cell(row, 0);

		sprintf(buffer, "Row %d", row);
		cell->Set(buffer);
		cell->SetFormat(fmt_bold);
	}

	for (row = 1; row <= row_max; ++row) {
		for (int col = 1; col <= col_max; ++col) {
			sprintf(buffer, "%d / %d", row, col);

			sheet->Cell(row, col)->Set(buffer);
		}
	}

	xls.SaveAs(path);
}

void test_excel()
{
	example1("example1.xls");
	example2("example2.xls");
	example3("example3.xls");
	example_read_write("example3.xls", "example3-out.xls");
	example4("example4.xls");

	//copy_sheet("example1.xls", "example2.xls");
	
	write_big_sheet("big-example.xls", 500, 100);
	cout << "test_excel" << endl;
	getchar();
}