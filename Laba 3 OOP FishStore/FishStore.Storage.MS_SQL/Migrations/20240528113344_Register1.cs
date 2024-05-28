using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace FishStore.Storage.MS_SQL.Migrations
{
    /// <inheritdoc />
    public partial class Register1 : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.RenameColumn(
                name: "SurName",
                table: "Users",
                newName: "Mail");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.RenameColumn(
                name: "Mail",
                table: "Users",
                newName: "SurName");
        }
    }
}
