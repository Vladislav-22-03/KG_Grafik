using FishStore.Storage.Database;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Design;

namespace FishStore.Storage.MS_SQL
{
    public class SqlServerContextFactory : IDesignTimeDbContextFactory<DataContext>
    {
        private const string DbContextString = "Server=localhost,1433;Database=FishStore;User ID=sa;Password=<YourStrong@Passw0rd>;MultipleActiveResultSets=true;TrustServerCertificate=True";


        public DataContext CreateDbContext(string[] args)
        {
            var optionBuilder = new DbContextOptionsBuilder<DataContext>();

            optionBuilder.UseSqlServer(DbContextString, b => b.MigrationsAssembly(typeof(SqlServerContextFactory).Namespace));

            return new DataContext(optionBuilder.Options);
        }
    }
}
