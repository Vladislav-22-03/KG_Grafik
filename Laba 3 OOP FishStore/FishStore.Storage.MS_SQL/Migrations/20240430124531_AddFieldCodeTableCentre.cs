using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace FishStore.Storage.MS_SQL.Migrations
{
    /// <inheritdoc />
    public partial class AddFieldCodeTableCentre : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "Code",
                table: "Centers",
                type: "nvarchar(255)",
                maxLength: 255,
                nullable: false,
                defaultValue: "");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "Code",
                table: "Centers");
        }
    }
}
