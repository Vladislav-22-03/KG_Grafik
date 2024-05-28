using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace FishStore.Storage.MS_SQL.Migrations
{
    /// <inheritdoc />
    public partial class Updatexyi : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropForeignKey(
                name: "FK_Users_Centers_IsnCenter",
                table: "Users");

            migrationBuilder.DropIndex(
                name: "IX_Users_IsnCenter",
                table: "Users");

            migrationBuilder.DropColumn(
                name: "IsnCenter",
                table: "Users");

            migrationBuilder.AlterColumn<string>(
                name: "Code",
                table: "Centers",
                type: "nvarchar(20)",
                maxLength: 20,
                nullable: false,
                oldClrType: typeof(string),
                oldType: "nvarchar(255)",
                oldMaxLength: 255);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<Guid>(
                name: "IsnCenter",
                table: "Users",
                type: "uniqueidentifier",
                nullable: false,
                defaultValue: new Guid("00000000-0000-0000-0000-000000000000"));

            migrationBuilder.AlterColumn<string>(
                name: "Code",
                table: "Centers",
                type: "nvarchar(255)",
                maxLength: 255,
                nullable: false,
                oldClrType: typeof(string),
                oldType: "nvarchar(20)",
                oldMaxLength: 20);

            migrationBuilder.CreateIndex(
                name: "IX_Users_IsnCenter",
                table: "Users",
                column: "IsnCenter");

            migrationBuilder.AddForeignKey(
                name: "FK_Users_Centers_IsnCenter",
                table: "Users",
                column: "IsnCenter",
                principalTable: "Centers",
                principalColumn: "IsnNode",
                onDelete: ReferentialAction.Cascade);
        }
    }
}
