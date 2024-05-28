using FishStore.Storage.Models;
using Microsoft.EntityFrameworkCore;

namespace FishStore.Storage.Database
{
    public class DataContext : DbContext
    {
        public DataContext(DbContextOptions options) : base(options)
        {
        }

        public virtual DbSet<Center> Centers { get; set; }

        public virtual DbSet<User> Users { get; set; }
    }
}
